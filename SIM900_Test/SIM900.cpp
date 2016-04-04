#include "SIM900.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "string.h"
#include "Tracer.h"
#include "CommonData.h"
#include "Pc.h"

#define PWR_PIN 7
#define SERIAL_DEFAULT_TIMEOUT  1000// ms

extern Tracer_Class tracer;

extern char* fToStr(char* buf, float val, char* str);

bool checkAns(char* ans, unsigned long timeout);

SoftwareSerial SIM900_Serial(2, 3);

void SIM900_Class::init(void)
{
  int8_t rdy = 0;
  SIM900_Serial.begin(9600);
  SIM900_Serial.setTimeout(SERIAL_DEFAULT_TIMEOUT * 7);
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
  //if(checkAns("OK", 2000))
  {
    return true;
    //SIM900_Serial.write(buf, sizeof(buf));
  }
  return false;
}

bool checkAns(char* ans, unsigned long timeOut)
{
  timeOut += millis();
  int cntA = 0;
  int ansLen = strlen(ans);

  SIM900_Serial.setTimeout(timeOut);

  sprintf((char*)ioBuf, "ansLen=%d", ansLen);
  Serial.println((char*)ioBuf);
  sprintf((char*)ioBuf, "time0=%d", timeOut);
  Serial.println((char*)ioBuf);
  timeOut += millis();
  sprintf((char*)ioBuf, "time1=%d", timeOut);
  Serial.println((char*)ioBuf);

  
  while((cntA <= ansLen) && (1 == SIM900_Serial.readBytes(ioBuf, 1)))
  {
    if(timeOut < millis())
    {
      SIM900_Serial.setTimeout(SERIAL_DEFAULT_TIMEOUT);
      return false;
    }

    Serial.write(ioBuf[0]);
    Serial.println("from checkAns");
    if(ans[cntA] == ioBuf[0])
    {
      if(cntA == ansLen)
      {
        Serial.println("true");
        SIM900_Serial.setTimeout(SERIAL_DEFAULT_TIMEOUT);
        return true;
      }
      cntA++;
    }else
    {
      cntA = 0;
    }
  }
  Serial.println("false");
  SIM900_Serial.setTimeout(SERIAL_DEFAULT_TIMEOUT);
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

void SIM900_Class::writeGetStart(void)
{
  /* Sena data to the server here */
  SIM900_Serial.write("AT+HTTPPARA=\"URL\",\"", 19);
  Serial.write("AT+HTTPPARA=\"URL\",\"", 19);
  SIM900_Serial.write(Settings.serverPath, strlen(Settings.serverPath));
  Serial.write(Settings.serverPath, strlen(Settings.serverPath));
  SIM900_Serial.write('?');
  Serial.write('?');
}

bool SIM900_Class::writeRatedData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  sprintf((char*)ioBuf, "&valType=rated");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  // Rated data
  for(int i = PvRatedVolt; i <= LoadRatedCur; i ++)
  {
    sprintf((char*)ioBuf, "&val%d=", i);
    fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
    Serial.write((char*)ioBuf, strlen((char*)ioBuf));      
    delay(10);      
  }

  SIM900_Serial.println("\"");
  Serial.println("\"");

  if(!checkOk()) return false;

  SIM900_Serial.println("AT+HTTPACTION=0");
  Serial.println("AT+HTTPACTION=0");
  delay(5000);
  Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
return true;//checkOk();
}

bool SIM900_Class::writeRealTimeData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  sprintf((char*)ioBuf, "&valType=realtime");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  // Real time data
  for(int i = PvInVolt; i <= BatRealRatedVolt; i ++)
  {
    sprintf((char*)ioBuf, "&val%d=", i);
    fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
    Serial.write((char*)ioBuf, strlen((char*)ioBuf));
      
    delay(5);      
  }

  SIM900_Serial.println("\"");
  Serial.println("\"");
  if(!checkOk()) return false;
  SIM900_Serial.println("AT+HTTPACTION=0");
  Serial.println("AT+HTTPACTION=0");
  delay(5000);
  Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
return true;
}

bool SIM900_Class::writeStatisticData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  sprintf((char*)ioBuf, "&valType=stat");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  // Staticstics data
  for(int i = MaxVoltToday; i <= BatCur; i ++)
  {
    sprintf((char*)ioBuf, "&val%d=", i);
    fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
    Serial.write((char*)ioBuf, strlen((char*)ioBuf));
      
    delay(5);      
  }

  SIM900_Serial.println("\"");
  Serial.println("\"");
  if(!checkOk()) return false;
  SIM900_Serial.println("AT+HTTPACTION=0");
  Serial.println("AT+HTTPACTION=0");
  delay(5000);
  Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
return true;//checkOk();
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
  delay(5000);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPINIT");
Serial.println("AT+HTTPINIT");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPPARA=\"CID\",1");
Serial.println("AT+HTTPPARA=\"CID\",1");
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;

/*****************************************************************************************************************/
if(!writeRatedData() || !writeRealTimeData() || !writeStatisticData())
{
  Serial.println("writeError");
  return SIM900_NO_ANS;
}
/*****************************************************************************************************************/

delay(2000);
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

