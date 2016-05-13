#include "modRtuCrcLib.h"
#include "CommonData.h"
#include "stdio.h"

#define PRINT_DEBUG 1
#define SEC_TO_MILLIS 1000
#define RESTART_TIMEOUT (millis() + (uint32_t)Settings.timeout * SEC_TO_MILLIS)

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
  CD_Cr.SendData = false;
  CD_Cr.RefreshTracer = true;
  pc.init();
  Serial.println("Settings init");
  Settings.init();
  Serial.println("SIM900 init");
  sim900.init();
  Serial.println("SIM900 power off");
  sim900.turnPower(false);
  Serial.println("Tracer init");
  tracer.init();
  Serial.println("Arduino started");
}

void loop() {
  uint16_t reg = 0;
  char tmp;
  static uint8_t tracerCnt = 0;
  static uint32_t sendTime = RESTART_TIMEOUT;

  noInterrupts();
  // restart millis counter
  timer0_overflow_count = 0;
  interrupts();

#if PRINT_DEBUG != 0
  // Check timeout
  sprintf((char*) ioBuf, "Timeout %u, Time left %lu.", Settings.timeout, sendTime - millis());
  Serial.println((char*)ioBuf);
#endif
  if(!pc.connected && sendTime < millis())
  {// Allow data sending
    #if PRINT_DEBUG != 0
    Serial.println("Allow data sending");
    #endif
    CD_Cr.SendData = true;
  }
  // Check command from PC
  if(pc.getCmd())
  {
    #if PRINT_DEBUG != 0
    Serial.println("Cmd");
    #endif
  }
  if(pc.connected)
  {
    if(++tracerCnt >= 20)
    {
      CD_Cr.RefreshTracer = true;
      tracerCnt = 0;
    }
    sendTime = RESTART_TIMEOUT;
    #if PRINT_DEBUG != 0
    sprintf((char*) ioBuf, "Pc connected. Restart in %u ms.", sendTime - millis());
    Serial.println((char*)ioBuf);
    #endif
  }else
  {
    CD_Cr.RefreshTracer = true;
    tracerCnt = 0;
  }

  // Get tracer data
  if(CD_Cr.RefreshTracer)
  {
    tracer.refreshData();
    CD_Cr.RefreshTracer = false;
  }
  
  if(CD_Cr.SendData)
  {// if data send command or send time expired
    if(sim900.turnPower(true))
    {
      sim900.checkAns("Call Ready", 60000);
      sim900.getSigLevel();
      sim900.checkAccount();
      sim900.getNumber();
      sim900.sendRealTimeData();
      #if PRINT_DEBUG != 0
      Serial.println("Turning off");
      #endif
      if(!sim900.turnPower(false))
      {
        #if PRINT_DEBUG != 0
        Serial.println("Turning off error");
        #endif
      }
        CD_Cr.SendData = false;
        sendTime = RESTART_TIMEOUT;
        #if PRINT_DEBUG != 0
        sprintf((char*) ioBuf, "Restart in %u ms.", sendTime - millis());
        Serial.println((char*)ioBuf);
        #endif
    }else
    {
      #if PRINT_DEBUG != 0
      Serial.println("Turning on error");
      #endif
    }
  }
  delay(10);
}
