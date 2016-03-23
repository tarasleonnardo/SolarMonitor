#include "SIM900.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "string.h"
#include "Tracer.h"
#include "CommonData.h"
#include "Pc.h"

#define PWR_PIN 7

extern Tracer_Class tracer;

extern char* fToStr(char* buf, float val, char* str);

SoftwareSerial SIM900_Serial(2, 3);

void SIM900_Class::init(void)
{
  int8_t rdy = 0;
  SIM900_Serial.begin(9600);
  SIM900_Serial.setTimeout(10000);
  pinMode(PWR_PIN, OUTPUT);
  
  while(rdy < 5)
  {
    digitalWrite(PWR_PIN, LOW);
    delay(1500);
    digitalWrite(PWR_PIN, HIGH);
    delay(2000);

    if(!isOn())
    continue;
    
    SIM900_Serial.readBytes(buf, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;

    if(strstr(buf, "Call Ready"))
    {
      Serial.println("SIM900 ready");
      SIM900_Serial.setTimeout(1500);
      break;
    }
    Serial.write('0' + rdy);
    rdy++;
    delay(1500);
  }

  if(rdy >= 5) Serial.println("SIM900 not ready");
}

void SIM900_Class::powerSwitch(void)
{
  digitalWrite(PWR_PIN, LOW);
  delay(1300);
  digitalWrite(PWR_PIN, HIGH);
}

void SIM900_Class::startListening(void)
{
  SIM900_Serial.listen();
}

bool SIM900_Class::isOn(void)
{
  SIM900_Serial.println("AT");
  if(checkOk())
  {
    return true;
    //SIM900_Serial.write(buf, sizeof(buf));
  }
  return false;
}

bool SIM900_Class::checkOk(void)
{
  int cnt = 0;

  while(1 == SIM900_Serial.readBytes(buf, 1))
  {// while there are data to read
      if('O' == buf[0])
      {
        Serial.println("O");
        searchK:
        if(1 == SIM900_Serial.readBytes(buf, 1))
        {
          if('K' == buf[0])
          {
            Serial.println("K");
            SIM900_Serial.readBytes(buf, 2); // read cr + lf to free input buffer
            return true;
          }else if('O' == buf[0])
          {
            goto searchK;
          }
        }else
        {
          return false;
        }
      }      
  }
  Serial.println("No data");
  return false;
}

SIM900_Class::ErrorsType SIM900_Class::sendRealTimeData(void)
{
  char* str = 0;
  int del = 1000;

  SIM900_Serial.println("AT+CIPSHUT");
  Serial.println("AT+CIPSHUT");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
  
  SIM900_Serial.println("AT+CGATT?");
  Serial.println("AT+CGATT?");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
  

SIM900_Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+SAPBR=3,1,\"APN\",\"internet\"");
Serial.println("AT+SAPBR=3,1,\"APN\",\"internet\"");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+SAPBR=3,1,\"USER\",\"\"");
Serial.println("AT+SAPBR=3,1,\"USER\",\"\"");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+SAPBR=3,1,\"PWD\",\"\"");
Serial.println("AT+SAPBR=3,1,\"PWD\",\"\"");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;

SIM900_Serial.println("AT+SAPBR=1,1");
Serial.println("AT+SAPBR=1,1");
  delay(10000);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPINIT");
Serial.println("AT+HTTPINIT");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPPARA=\"CID\",1");
Serial.println("AT+HTTPPARA=\"CID\",1");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;

/* Sena data to the server here */
SIM900_Serial.write("AT+HTTPPARA=\"URL\",\"", 19);
Serial.write("AT+HTTPPARA=\"URL\",\"", 19);
SIM900_Serial.write(Settings.serverPath, strlen(Settings.serverPath));
Serial.write(Settings.serverPath, strlen(Settings.serverPath));
SIM900_Serial.write('?');
Serial.write('?');
  
for(int i = PvInVolt; i <= BatRealRatedVolt/*TracerDataNum*/; i ++)
{
  if(i > PvInVolt)
    sprintf((char*)ioBuf, "&val%d=", i);
  else
   sprintf((char*)ioBuf, "val%d=", i);
  fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  Serial.write((char*)ioBuf, strlen((char*)ioBuf));
    
  delay(5);      
}

SIM900_Serial.println("\"");
Serial.println("\"");

  delay(del + del + del);
  SIM900_Serial.readBytes(buf, sizeof(buf) - 1);
  
SIM900_Serial.println("AT+HTTPACTION=0");
Serial.println("AT+HTTPACTION=0");
  delay(10000);
  Serial.setTimeout(10000);
  Serial.write(buf, SIM900_Serial.readBytes(buf, sizeof(buf) - 1));
  Serial.setTimeout(1500);
  
  //if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPTERM");
Serial.println("AT+HTTPTERM");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;

return SIM900_NO_ERR;
}

bool SIM900_Class::checkAccount(void)
{
  int cnt = 0;
  unsigned long time = 0;
  char* money = 0;
  
  SIM900_Serial.println("AT+CUSD=1,\"*111#\"");
  Serial.println("AT+CUSD=1,\"*111#\"");
  if(checkOk())
  {
    Serial.println("Wait for data");
    time = millis() + 60000;
    while(time > millis() && SIM900_Serial.available() == 0); // wait for data

    //if(SIM900_Serial.available() > 0)
      cnt = SIM900_Serial.readBytes(buf, sizeof(buf));
      Serial.write(buf, cnt);

      money = strstr(buf, "grn");
      if(money != 0)
      {
        if(*(money - 1) == ' ')
        {
          money -= 1;
        }

        money -= 5;

        if(*money == ' ')
        money ++;
        Serial.write(money, 5);
        balans = atof(money);
        
      }else
      {
        Serial.println("Money not found");
      }
      
      return true;
  }else
  {
    Serial.println("USSD ERR");
  }

  return false;
}

bool SIM900_Class::getNumber(void)
{
  int cnt = 0;
  unsigned long time = 0;
  char* num = 0;
  
  SIM900_Serial.println("AT+CUSD=1,\"*161#\"");
  Serial.println("AT+CUSD=1,\"*161#\"");
  if(checkOk())
  {
    Serial.println("Wait for data");
    time = millis() + 60000;
    while(time > millis() && SIM900_Serial.available() == 0); // wait for data

    //if(SIM900_Serial.available() > 0)
      cnt = SIM900_Serial.readBytes(buf, sizeof(buf));
      Serial.write(buf, cnt);

      num = strstr(buf, "380");

      if(num != 0)
      {
        Serial.write(num, 13);
      }else
      {
        Serial.println("Number not found");
      }
      return true;
  }else
  {
    Serial.println("USSD ERR");
  }

  return false;
}

