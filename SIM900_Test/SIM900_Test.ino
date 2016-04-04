#include "modRtuCrcLib.h"
#include "CommonData.h"
#include "stdio.h"



char* fToStr(char* buf, float val, char* str)
{
  int32_t tmp = (int32_t) val;
  int8_t cnt = 0;

  if(str != NULL) sprintf(buf, str);
  else buf[0] = '\0';

  cnt = strlen(buf);
  sprintf(buf + cnt, "%d.", tmp);

  cnt = strlen(buf);
  tmp = (int32_t) ((int32_t)(val * 10) % 10);
  buf[cnt++] = (char)tmp + '0';
  
  tmp = (int32_t) ((int32_t)(val * 100) % 10);
  buf[cnt++] = (char)tmp + '0';
  
  tmp = (int32_t) ((int32_t)(val * 1000) % 10);
  buf[cnt++] = (char)tmp + '0';

  buf[cnt] = '\0';

  return buf;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(6, OUTPUT);

  CD_Cr.SendData = false;
  Settings.init();
  tracer.init();
  pc.init();
}

void loop() {
  uint16_t reg = 0;
  char tmp;
  static unsigned long sendTime = 0;
  // put your main code here, to run repeatedly:
  if(0 == digitalRead(13))
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
  

  tracer.startListening();
  if(tracer.refreshReadOnlyData())
  {
   // Serial.println("Tracer data Ok!");
    /*
    Serial.println("*********************");
    for(int i = 0; i < TracerDataNum; i ++)
    {
      sprintf((char*)ioBuf, "Val%d=", i);
      fToStr((char*)(ioBuf + strlen((char*)ioBuf)), CD_TracerData[i], "");
      Serial.println((char*)ioBuf);
      delay(5);      
    }
    */
  }else
  {
    Serial.println("Tracer data Error!");
    delay(100);
  }
  pc.getCmd();

  if(sendTime < millis())
  {
    sendTime = millis() + Settings.period * 60000;
    CD_Cr.SendData = true;  
  }

  if(CD_Cr.SendData)
  {
    sim900.init();
    if(sim900.isOn())
    {
      sim900.checkAccount();
      //sim900.getNumber();
      if(SIM900_Class::SIM900_NO_ERR == sim900.sendRealTimeData())
      {
        Serial.println("Data sending OK!");
      }else
      {
        Serial.println("Data sending Error!");
      }
      sim900.powerSwitch();
    }
    CD_Cr.SendData = false;
  }

  return;
}
