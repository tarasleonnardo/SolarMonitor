#include "SIM900.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "string.h"
#include "Tracer.h"
#include "CommonData.h"
#include "Pc.h"

#define SIM900_PRINT_DEBUG  1
#define PWR_PIN   7
#define SERIAL_DEFAULT_TIMEOUT  1000// ms

#define STATUS_HIGH (0 != digitalRead(STAT_PIN))

extern Tracer_Class tracer;

extern char* fToStr(char* buf, float val, char* str);

SoftwareSerial SIM900_Serial(2, 3);

bool SIM900_Class::turnPower(bool on)
{
  if(on)
  {// Turn on here
      if(!isOn())
      {
        digitalWrite(PWR_PIN, LOW);
        delay(1350);
        digitalWrite(PWR_PIN, HIGH);
        delay(500);
      }
      return isOn();      
  }else
  {// Turn off here
    if(isOn())
    {
      digitalWrite(PWR_PIN, LOW);
      delay(1350);
      digitalWrite(PWR_PIN, HIGH);
      delay(20);
    }
    return checkAns("POWER DOWN", 1000);
  }
}

void SIM900_Class::init(void)
{
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  SIM900_Serial.begin(9600);
}

void SIM900_Class::listen(void)
{
  SIM900_Serial.listen();
}

bool SIM900_Class::checkAns(char* ans, unsigned long timeout)
{
    int16_t cnt = 0;
    int16_t end = strlen(ans);
    char difCase = 0;
    
    timeout = millis() + timeout;
    
    do
    {
      if(1 == SIM900_Serial.readBytes(ioBuf, 1))
      {
        // Remember character in other case to check case-independent answer
        if(0 != isupper(ans[cnt]))
        {
            difCase = tolower(ans[cnt]);
        }else
        {
            difCase = toupper(ans[cnt]);
        }

        if(ioBuf[0] == ans[cnt] || ioBuf[0] == difCase)
        {// If received character from ans
            // Switch to the next character
            cnt++;
            continue;
        }
      }
        // If character is not from ans
        // Look for the first character
        cnt = 0;
        if(timeout < millis())
        {
            return false;
        }
    }while(cnt < end);
return true;
}

bool SIM900_Class::isOn(void)
{
  sim900.listen();
  discardBuffer();
  SIM900_Serial.println("AT");
  if(checkAns("OK", 1000))
  {
    return true;
  }
  return false;
}

void SIM900_Class::discardBuffer(void)
{
  while(0 < SIM900_Serial.available())
  {
    SIM900_Serial.read();
  }
}

bool SIM900_Class::checkOk(void)
{
  int cnt = 0;

  while(1 == SIM900_Serial.readBytes(buf, 1))
  {// while there are data to read
      if('O' == buf[0])
      {
       #if SIM900_PRINT_DEBUG == 1
       Serial.println("O");
       #endif
        searchK:
        if(1 == SIM900_Serial.readBytes(buf, 1))
        {
          if('K' == buf[0])
          {
           #if SIM900_PRINT_DEBUG == 1
           Serial.println("K");
           #endif
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
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("No data");
 #endif
  return false;
}

void SIM900_Class::writeGetStart(void)
{
  /* Sena data to the server here */
  SIM900_Serial.write("AT+HTTPPARA=\"URL\",\"", 19);
 #if SIM900_PRINT_DEBUG == 1
 Serial.write("AT+HTTPPARA=\"URL\",\"", 19);
 #endif
  delay(10);
  SIM900_Serial.write(Settings.serverPath, strlen(Settings.serverPath));
 #if SIM900_PRINT_DEBUG == 1
 Serial.write(Settings.serverPath, strlen(Settings.serverPath));
 #endif
  delay(10);
  SIM900_Serial.write('?');
 #if SIM900_PRINT_DEBUG == 1 
  Serial.write('?');
 #endif
}

bool SIM900_Class::writeRatedData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  sprintf((char*)ioBuf, "&valType=rated");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  // Rated data
  for(int i = PvRatedVolt; i <= LoadRatedCur; i ++)
  {
    sprintf((char*)ioBuf, "&val%d=", i);
    fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #if SIM900_PRINT_DEBUG == 1
   Serial.write((char*)ioBuf, strlen((char*)ioBuf));       
   #endif
    delay(10);      
  }

  SIM900_Serial.println("\"");
 #if SIM900_PRINT_DEBUG == 1
 Serial.println("\"");
 #endif
  delay(10);
  if(!checkOk()) return false;

  SIM900_Serial.println("AT+HTTPACTION=0");
 #if SIM900_PRINT_DEBUG == 1
 Serial.println("AT+HTTPACTION=0"); 
 #endif
  delay(5000);
 #if SIM900_PRINT_DEBUG == 1
 Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
 #endif
return true;//checkOk();
}

bool SIM900_Class::writeRealTimeData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  sprintf((char*)ioBuf, "&valType=realtime");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  // Real time data
  for(int i = PvInVolt; i <= BatRealRatedVolt; i ++)
  {
    sprintf((char*)ioBuf, "&val%d=", i);
    fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #if SIM900_PRINT_DEBUG == 1
   Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #endif
      
    delay(5);      
  }

  SIM900_Serial.println("\"");
 #if SIM900_PRINT_DEBUG == 1
 Serial.println("\"");
 #endif
  delay(10);
  if(!checkOk()) return false;
  SIM900_Serial.println("AT+HTTPACTION=0");
 #if SIM900_PRINT_DEBUG == 1
 Serial.println("AT+HTTPACTION=0");
 #endif
  delay(5000);
 #if SIM900_PRINT_DEBUG == 1
 Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
 #endif
return true;
}

bool SIM900_Class::writeStatisticData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  sprintf((char*)ioBuf, "&valType=stat");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  delay(10);
  // Staticstics data
  for(int i = MaxVoltToday; i <= BatCur; i ++)
  {
    sprintf((char*)ioBuf, "&val%d=", i);
    fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #if SIM900_PRINT_DEBUG == 1
   Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #endif
    delay(5);      
  }

  SIM900_Serial.println("\"");
 #if SIM900_PRINT_DEBUG == 1
 Serial.println("\"");
 #endif
  delay(10);
  if(!checkOk()) return false;
  SIM900_Serial.println("AT+HTTPACTION=0");
 #if SIM900_PRINT_DEBUG == 1
 Serial.println("AT+HTTPACTION=0");
 #endif
  delay(5000);
 #if SIM900_PRINT_DEBUG == 1
 Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
 #endif
return true;//checkOk();
}

bool SIM900_Class::writeGsmData(void)
{
  writeGetStart();
  sprintf((char*)ioBuf, "usr=%s", Settings.usrName);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  #if SIM900_PRINT_DEBUG != 0
  Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  #endif
  delay(10);
  sprintf((char*)ioBuf, "&pass=%s", Settings.usrPwd);
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  #if SIM900_PRINT_DEBUG != 0
  Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  #endif
  delay(10);
  sprintf((char*)ioBuf, "&valType=gsm");
  SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  #if SIM900_PRINT_DEBUG != 0
  Serial.write((char*)ioBuf, strlen((char*)ioBuf));
  #endif
  delay(10);
  // gsm data
  // balans
    fToStr((char*)(ioBuf), balans, "&val33=");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #if SIM900_PRINT_DEBUG != 0
   Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #endif
    delay(10);
  // phone number
    sprintf((char*)ioBuf, "&val34=%s", phoneNum);
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
   #if SIM900_PRINT_DEBUG != 0 
   Serial.write((char*)ioBuf, strlen((char*)ioBuf)); 
   #endif
    delay(10);

  // signal level
    fToStr((char*)ioBuf, signalLevel, "&val35=");
    SIM900_Serial.write((char*)ioBuf, strlen((char*)ioBuf));
    SIM900_Serial.write("%");
   #if SIM900_PRINT_DEBUG != 0 
   Serial.write((char*)ioBuf, strlen((char*)ioBuf)); 
   Serial.write("%");   
   #endif
    delay(10);

  SIM900_Serial.println("\"");
 #if SIM900_PRINT_DEBUG != 0
 Serial.println("\"");
 #endif
  delay(10);
  if(!checkOk()) return false;
  SIM900_Serial.println("AT+HTTPACTION=0");
 #if SIM900_PRINT_DEBUG != 1 
 Serial.println("AT+HTTPACTION=0"); 
 #endif
  delay(7000);
 //#if SIM900_PRINT_DEBUG == 1 
 Serial.write(ioBuf, SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE));
 delay(10);
 //#else
 //SIM900_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE); 
 //#endif
return true;//checkOk();
}

SIM900_Class::ErrorsType SIM900_Class::sendRealTimeData(void)
{
  char* str = 0;
  int del = 2000;

  SIM900_Serial.listen();
  SIM900_Serial.println("AT+CIPSHUT");
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("AT+CIPSHUT"); 
 #endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
  
  SIM900_Serial.println("AT+CGATT?");
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("AT+CGATT?"); 
 #endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
  

SIM900_Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
#endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+SAPBR=3,1,\"APN\",\"internet\"");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+SAPBR=3,1,\"APN\",\"internet\""); 
#endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+SAPBR=3,1,\"USER\",\"\"");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+SAPBR=3,1,\"USER\",\"\""); 
#endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+SAPBR=3,1,\"PWD\",\"\"");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+SAPBR=3,1,\"PWD\",\"\""); 
#endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;

SIM900_Serial.println("AT+SAPBR=1,1");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+SAPBR=1,1"); 
#endif
  delay(6000);
  while(SIM900_Serial.available())
  {
   #if SIM900_PRINT_DEBUG == 1 
   Serial.write(SIM900_Serial.read()); 
   #else
   SIM900_Serial.read();
   #endif
  }
  //if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPINIT");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+HTTPINIT"); 
#endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;
SIM900_Serial.println("AT+HTTPPARA=\"CID\",1");
#if SIM900_PRINT_DEBUG == 1
Serial.println("AT+HTTPPARA=\"CID\",1"); 
#endif
  delay(del);
  if(!checkOk()) return SIM900_NO_ANS;

/*****************************************************************************************************************/
if(!writeRatedData() || !writeRealTimeData() || !writeStatisticData() || !writeGsmData())
{
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("writeError"); 
 #endif
  return SIM900_NO_ANS;
}
/*****************************************************************************************************************/

delay(2000);
  SIM900_Serial.println("AT+HTTPTERM");
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("AT+HTTPTERM"); 
 #endif
    delay(del);
    if(!checkOk()) return SIM900_NO_ANS;


return SIM900_NO_ERR;
}

bool SIM900_Class::checkAccount(void)
{
  int cnt = 0;
  unsigned long time = 0;
  char* money = 0;

  discardBuffer();
  SIM900_Serial.println("AT+CUSD=1,\"*111#\"");
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("AT+CUSD=1,\"*111#\""); 
 #endif
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("Wait for data"); 
 #endif
  if(checkAns("CUSD:", 60000))
  {
    //if(SIM900_Serial.available() > 0)
      cnt = SIM900_Serial.readBytes(buf, sizeof(buf));
     #if SIM900_PRINT_DEBUG == 1 
     Serial.write(buf, cnt); 
     #endif
      delay(5);
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
       #if SIM900_PRINT_DEBUG == 1 
       Serial.write(money, 5); 
       delay(5); 
       #endif
        balans = atof(money);
      }else
      {
        balans = -1;
       #if SIM900_PRINT_DEBUG == 1 
       Serial.println("Money not found"); 
       #endif
      }
      
      return true;
  }else
  {
   #if SIM900_PRINT_DEBUG == 1 
   Serial.println("USSD ERR");
   #endif
  }

  return false;
}

bool SIM900_Class::getNumber(void)
{
  int cnt = 0;
  unsigned long time = 0;
  char* num = 0;

  discardBuffer();
  SIM900_Serial.println("AT+CUSD=1,\"*161#\"");
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("AT+CUSD=1,\"*161#\""); 
 Serial.println("Wait for data"); 
 #endif
  if(checkAns("CUSD:", 60000))
  {
    //if(SIM900_Serial.available() > 0)
      cnt = SIM900_Serial.readBytes(buf, sizeof(buf));
     #if SIM900_PRINT_DEBUG == 1 
     Serial.write(buf, cnt);
     #endif
      num = strstr(buf, "380");

      if(num != 0)
      {
        memcpy(phoneNum, num, 12);
        phoneNum[12] = '\0';
       #if SIM900_PRINT_DEBUG == 1
       Serial.println("Num is ");
       delay(3);
       Serial.println(phoneNum); 
       #endif
      }else
      {
        #if SIM900_PRINT_DEBUG == 1 
        sprintf(phoneNum, "unknown");
        Serial.println("Number not found"); 
        #endif
      }
      return true;
  }else
  {
   #if SIM900_PRINT_DEBUG == 1 
   Serial.println("USSD ERR"); 
   #endif
  }

  return false;
}

bool SIM900_Class::getSigLevel(void)
{
  int cnt = 0;
  unsigned long time = 0;
  char* num = 0;

  discardBuffer();
  SIM900_Serial.println("AT+CSQ");
 #if SIM900_PRINT_DEBUG == 1 
 Serial.println("AT+CSQ");
 Serial.println("Wait for data"); 
 #endif
  if(checkAns("CSQ:", 60000))
  {
    //if(SIM900_Serial.available() > 0)
      cnt = SIM900_Serial.readBytes(buf, sizeof(buf));
     #if SIM900_PRINT_DEBUG == 1 
     Serial.write(buf, cnt); 
     #endif

      num = buf;

      if(!isdigit(*num))
      {
        num++;
      }
      if(!isdigit(*num))
      {
        num++;
      }
      if(!isdigit(*num))
      {
       #if SIM900_PRINT_DEBUG == 1 
       Serial.println("Digits not found"); 
       #endif
        return false;
      }
      cnt = atoi(num);

      if(cnt == 99)
      {
        signalLevel = 0;
      }else
      {// 31 is 100 %
        signalLevel = 99.f / 31 * cnt;
      }
     #if SIM900_PRINT_DEBUG == 1 
     Serial.println(fToStr((char*)ioBuf, signalLevel, "Signal "));
     #endif
      return true;
  }else
  {
   #if SIM900_PRINT_DEBUG == 1 
   Serial.println("USSD ERR"); 
   #endif
  }

  return false;
}


