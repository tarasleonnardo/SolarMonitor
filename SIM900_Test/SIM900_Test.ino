#include "modRtuCrcLib.h"
#include "CommonData.h"
#include "stdio.h"



char* fToStr(char* buf, float val, char* str)
{
  uint32_t tmp = (uint32_t) val;

  if(str != NULL) sprintf(buf, str);
  else buf[0] = '\0';

  
  sprintf(buf + strlen(buf), "%d", tmp);

  tmp = (uint32_t) ((val - tmp) * 10000);

  sprintf(buf + strlen(buf), ".%d", tmp);

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
  // put your main code here, to run repeatedly:
  if(0 == digitalRead(13))
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
  

  tracer.startListening();
  tracer.refreshRatedData();
  tracer.refreshRealTimeData();

  pc.getCmd();

  if(CD_Cr.SendData)
  {
    sim900.init();
    if(sim900.isOn())
    {
      //sim900.checkAccount();
      //sim900.getNumber();
      sim900.sendRealTimeData();
    }
    CD_Cr.SendData = false;
  }

  return;
}
