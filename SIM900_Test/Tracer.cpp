#include "Tracer.h"
#include "Arduino.h"
#include "modRtuCrcLib.h"

SoftwareSerial Tracer_Serial(4, 5);

#define TRACER_RW_PIN   6
#define TRACER_SWITCH_TO_SEND  digitalWrite(TRACER_RW_PIN, HIGH);
#define TRACER_SWITCH_TO_RECV  digitalWrite(TRACER_RW_PIN, LOW);

void Tracer_Class::init(void)
{
  pinMode(TRACER_RW_PIN, OUTPUT);
  TRACER_SWITCH_TO_RECV
  Tracer_Serial.begin(115200);
  #ifdef TRACER_DEBUG
  Tracer_Serial.println("Tracer uart started");
  #endif
}

void Tracer_Class::startListening(void)
{
  Tracer_Serial.listen();
}

bool Tracer_Class::getRegister(uint16_t* reg, REGISTERS_ADDRESSES regN)
{
  int8_t cnt = 0;
  while(cnt++ < 5)
  {
    formAskFrame(regN, 1); // form ask to receive one register
    send(); // send ask
    delay(10); // wait for answer
    if(true != receive())
    {
      continue;
    }
    *reg = buf[4];
    *reg |= buf[3] << 8;
    return true;
  }
  return false;
}
 

/*************************************************/
/* Sends buffer to slave */
/*************************************************/
void Tracer_Class::send(void)
{
  TRACER_SWITCH_TO_SEND
  Tracer_Serial.write(buf, bufCount);
  //delayMicroseconds(336 * (bufCount));
  TRACER_SWITCH_TO_RECV
}

/*************************************************/
/* Receives data from slave and checks crc */
/*************************************************/
bool Tracer_Class::receive(void)
{
  uint16_t crc = 0;
  uint8_t cnt = 0;
  
  if(0 == Tracer_Serial.available())
  {
    return false;
  }
  else if(TRACER_BUF_SIZE >= Tracer_Serial.available())
  {
    cnt = Tracer_Serial.readBytes(buf, Tracer_Serial.available());
  }else
  {
    cnt = Tracer_Serial.readBytes(buf, TRACER_BUF_SIZE);
  }
  bufCount = cnt;
  cnt = 0;  
  // find frame start
  while((buf[cnt] != MB_SLAVE_ADDR) &&
        (buf[cnt + 1] != lastCmd))
  {
    cnt++;
    if(cnt >= bufCount - 5)return false;
  }
  if(cnt != 0)
  {// shift frame to the left
    memcpy(buf, buf + cnt, sizeof(buf) - cnt);
  }
  bufCount = buf[2] + 5;
  
  crc = buf[bufCount - 2];
  crc |= (buf[bufCount - 1] << 8);

  return (CRC_Ok == CRC_ModRtuCrcCheck(crc, buf, bufCount - 2));
}

/*************************************************/
/* Make frame to send with crc in the buffer */
/*************************************************/
void Tracer_Class::formAskFrame(REGISTERS_ADDRESSES val, uint16_t recLength)
{
  bufCount = 0;

  uint16_t crc = 0;

  buf[bufCount++] = MB_SLAVE_ADDR; // Slave address
  buf[bufCount++] = lastCmd = MB_CMD_READ_INPUT_REGS; // Read few store registers command

  crc = (uint16_t) val;
  buf[bufCount++] = (crc >> 8) & 0xFF; // Register address high
  buf[bufCount++] = crc & 0xFF; // Register address low

  buf[bufCount++] = (recLength >> 8) & 0xFF;
  buf[bufCount++] = recLength & 0xFF;

  crc = CRC_ModRtuCrcCalc(buf, bufCount);

  buf[bufCount++] = crc & 0xFF;
  buf[bufCount++] = (crc >> 8) & 0xFF;
}

bool Tracer_Class::refreshRealTimeData(void)
{
  uint16_t uiL = 0, uiH = 0;
  
    if(!getRegister(&uiL, ADDR_INPUT_VOLTAGE))
    return false;
    RealTimeData.ArrayInVolt = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_INPUT_CURRENT))
    return false;
    RealTimeData.ArrayInCur = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_INPUT_POWER_L) || 
       !getRegister(&uiL, ADDR_INPUT_POWER_H))
    return false;
    RatedData.ArrayRatedPow = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_BAT_CHRG_POWER_L) || 
       !getRegister(&uiL, ADDR_BAT_CHRG_POWER_H))
    return false;
    RealTimeData.BatPow = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_LOAD_VOLTAGE))
    return false;
    RealTimeData.LoadInVolt = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_LOAD_CURRENT))
    return false;
    RealTimeData.LoadInCur = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_LOAD_POWER_L) || 
       !getRegister(&uiL, ADDR_LOAD_POWER_H))
    return false;
    RealTimeData.LoadInPow = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_BAT_TEMP))
    return false;
    RealTimeData.BatTemp = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_INSIDE_TEMP))
    return false;
    RealTimeData.InsideTemp = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_BAT_SOC))
    return false;
    RealTimeData.BatSoc = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_BAT_REMOTE_TEMP))
    return false;
    RealTimeData.RemoteBatTemp = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_BAT_RE_RATED_POW))
    return false;
    RealTimeData.BatRealRatedPow = (float)uiL / TRACER_TIMES;

    if(!getRegister(&uiL, ADDR_BAT_STATUS))
    return false;
    RealTimeData.BatStatus = uiL;

    if(!getRegister(&uiL, ADDR_CHRG_EQUIPMENT_STATUS))
    return false;
    RealTimeData.ChrgEquipmentStatus = uiL;

    if(!getRegister(&uiL, ADDR_DISCHRG_EQUIPMENT_STATUS))
    return false;
    RealTimeData.DischargingEquipmentStatus = uiL;

    return true;
}

bool Tracer_Class::refreshRatedData(void)
{
  uint16_t uiL = 0, uiH = 0;

    if(!getRegister(&uiL, ADDR_PV_RATED_VOLTAGE)) // Voltage
    return false;
    RatedData.ArrayRatedVolt = (float)uiL / TRACER_TIMES;
    if(!getRegister(&uiL, ADDR_PV_RATED_CURRENT)) // Current
    return false;
    RatedData.ArrayRatedCur = (float)uiL / TRACER_TIMES;
    if(!getRegister(&uiL, ADDR_PV_RATED_POW_L)) // Power low
    return false;
    if(!getRegister(&uiH, ADDR_PV_RATED_POW_H)) // Power high
    return false;
    RatedData.ArrayRatedPow = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    if(!getRegister(&uiH, ADDR_BAT_VOLTAGE)) // Rated bat volt
    return false;
    RatedData.ArrayRatedVolt = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;
    if(!getRegister(&uiH, ADDR_BAT_CURRENT)) // Rated bat volt
    return false;
    RatedData.BatRatedCurr = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;
    if(!getRegister(&uiL, ADDR_BAT_POWER_L)) // Power low
    return false;
    if(!getRegister(&uiH, ADDR_BAT_POWER_H)) // Power high
    return false;
    RatedData.ArrayRatedPow = (float)(uiL | ((uint32_t)uiH << 16)) / TRACER_TIMES;
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    if(!getRegister(&uiL, ADDR_LOAD_RATED_CURRENT)) // Rated load current
    return false;
    RatedData.LoadRatedCurr = (float)uiL / TRACER_TIMES;
    
    if(!getRegister(&uiL, ADDR_CHARGING_MODE)) // Charging mode
    return false;
    RatedData.CrgMode = uiL;
    
    return true;
}



