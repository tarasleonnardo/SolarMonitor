#include "Tracer.h"
#include "Arduino.h"
#include "modRtuCrcLib.h"
#include "CommonData.h"

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

bool Tracer_Class::getValueDoubleReg(float* val, REGISTERS_ADDRESSES regnL, TRACER_MODBUS_ADDRESES slv)
{
  uint16_t regL, regH;
  
  if(getReadOnlyRegister(&regL, regnL, slv) && getReadOnlyRegister(&regH, (REGISTERS_ADDRESSES) (regnL + 1), slv))
  {
    *val = (regL | (regH << 16)) / TRACER_TIMES;
    return true;
  }

  return false;
}

bool Tracer_Class::getValueSingleReg(float* val, REGISTERS_ADDRESSES regN, TRACER_MODBUS_ADDRESES slv)
{
  uint16_t reg;
  
  if(getReadOnlyRegister(&reg, regN, slv))
  {
    *val = reg / TRACER_TIMES;
    return true;
  }

  return false;
}

bool Tracer_Class::getRwRegister(uint16_t* reg, REGISTERS_ADDRESSES regN, TRACER_MODBUS_ADDRESES slv)
{
  int8_t cnt = 0;
  while(cnt++ < 5)
  {
    formAskFrame(regN, MB_CMD_READ_STORE_REGS, slv, 1); // form ask to receive one register
    send(); // send ask
    delay(10); // wait for answer
    if(true != receive(slv))
    {
      continue;
    }
    *reg = ioBuf[4];
    *reg |= ioBuf[3] << 8;

    return true;
  }
  return false;
}

bool Tracer_Class::getFlagReg(uint16_t*reg, REGISTERS_ADDRESSES regN, TRACER_MODBUS_ADDRESES slv)
{
  int8_t cnt = 0;
  while(cnt++ < 5)
  {
    formAskFrame(regN, MB_CMD_READ_FLAGS_REGS, slv, 1); // form ask to receive one register
    send(); // send ask
    delay(10); // wait for answer
    if(true != receive(slv))
    {
      continue;
    }
    *reg = ioBuf[4];
    *reg |= ioBuf[3] << 8;

    return true;
  }
  return false;
}

bool Tracer_Class::getDigitalInput(uint16_t*reg, REGISTERS_ADDRESSES regN, TRACER_MODBUS_ADDRESES slv)
{
  int8_t cnt = 0;
  while(cnt++ < 5)
  {
    formAskFrame(regN, MB_CMD_READ_DIG_INPUTS, slv, 1); // form ask to receive one register
    send(); // send ask
    delay(10); // wait for answer
    if(true != receive(slv))
    {
      continue;
    }
    *reg = ioBuf[4];
    *reg |= ioBuf[3] << 8;

    return true;
  }
  return false;
}

bool Tracer_Class::getReadOnlyRegister(uint16_t* reg, REGISTERS_ADDRESSES regN, TRACER_MODBUS_ADDRESES slv)
{
  int8_t cnt = 0;
  while(cnt++ < 5)
  {
    formAskFrame(regN, MB_CMD_READ_INPUT_REGS, slv, 1); // form ask to receive one register
    send(); // send ask
    delay(10); // wait for answer
    if(true != receive(slv))
    {
      continue;
    }
    *reg = ioBuf[4];
    *reg |= ioBuf[3] << 8;

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
  Tracer_Serial.write(ioBuf, bufCount);
  //delayMicroseconds(336 * (bufCount));
  TRACER_SWITCH_TO_RECV
}

/*************************************************/
/* Receives data from slave and checks crc */
/*************************************************/
bool Tracer_Class::receive(TRACER_MODBUS_ADDRESES slv)
{
  uint16_t crc = 0;
  uint8_t cnt = 0;
  
  if(0 == Tracer_Serial.available())
  {
    return false;
  }
  else if(CD_IO_BUF_SIZE >= Tracer_Serial.available())
  {
    cnt = Tracer_Serial.readBytes(ioBuf, Tracer_Serial.available());
  }else
  {
    cnt = Tracer_Serial.readBytes(ioBuf, CD_IO_BUF_SIZE);
  }
  bufCount = cnt;
  cnt = 0;  
  // find frame start
  while((ioBuf[cnt] != slv) &&
        (ioBuf[cnt + 1] != lastCmd))
  {
    cnt++;
    if(cnt >= bufCount - 5)return false;
  }
  if(cnt != 0)
  {// shift frame to the left
    memcpy(ioBuf, ioBuf + cnt, sizeof(ioBuf) - cnt);
  }
  bufCount = ioBuf[2] + 5;
  
  crc = ioBuf[bufCount - 2];
  crc |= (ioBuf[bufCount - 1] << 8);

  return (CRC_Ok == CRC_ModRtuCrcCheck(crc, ioBuf, bufCount - 2));
}

/*************************************************/
/* Make frame to send with crc in the buffer */
/*************************************************/
void Tracer_Class::formAskFrame(REGISTERS_ADDRESSES val, MODBUS_CMDS cmd, TRACER_MODBUS_ADDRESES slv, uint16_t recLength)
{
  bufCount = 0;

  uint16_t crc = 0;

  ioBuf[bufCount++] = slv; // Slave address
  ioBuf[bufCount++] = lastCmd = cmd;

  crc = (uint16_t) val;
  ioBuf[bufCount++] = (crc >> 8) & 0xFF; // Register address high
  ioBuf[bufCount++] = crc & 0xFF; // Register address low

  ioBuf[bufCount++] = (recLength >> 8) & 0xFF;
  ioBuf[bufCount++] = recLength & 0xFF;

  crc = CRC_ModRtuCrcCalc(ioBuf, bufCount);

  ioBuf[bufCount++] = crc & 0xFF;
  ioBuf[bufCount++] = (crc >> 8) & 0xFF;
}

bool Tracer_Class::refreshData(void)
{
  uint16_t uiL = 0, uiH = 0;

  Tracer_Serial.listen();
  
 // Rated data
  if(!getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::PvRatedVolt], ADDR_PV_RATED_VOLTAGE, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::PvRatedCur], ADDR_PV_RATED_CURRENT, MB_SLAVE_TRACER) ||
    !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::PvRatedPow], ADDR_PV_RATED_POW_L, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::BatRatedVolt], ADDR_BAT_RATED_VOLTAGE, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::BatRatedCur], ADDR_BAT_RATED_CURRENT, MB_SLAVE_TRACER) ||
    !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::BatRatedPow], ADDR_BAT_RATED_POWER_L, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::LoadRatedCur], ADDR_LOAD_RATED_CURRENT, MB_SLAVE_TRACER))
    return false;
    
  /*** Real-time data ***/

  // Panel data
  if(!getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::PvInVolt], ADDR_INPUT_VOLTAGE, MB_SLAVE_TRACER) ||
     !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::PvInCur], ADDR_INPUT_CURRENT, MB_SLAVE_TRACER) ||
     !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::PvInPow], ADDR_INPUT_POWER_L, MB_SLAVE_TRACER))
     return false;

  // Battery data
  if(!getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::BatPow], ADDR_BAT_CHRG_POWER_L, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::BatTemp], ADDR_BAT_TEMP, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::BatSoc], ADDR_BAT_SOC, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::RemoteBatTemp], ADDR_BAT_REMOTE_TEMP, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::BatRealRatedVolt], ADDR_BAT_RE_RATED_VOLT, MB_SLAVE_TRACER))
    return false;
  
  // Load data
  if(!getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::LoadVolt], ADDR_LOAD_VOLTAGE, MB_SLAVE_TRACER) ||
    !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::LoadCur], ADDR_LOAD_CURRENT, MB_SLAVE_TRACER) ||
    !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::LoadPow], ADDR_LOAD_POWER_L, MB_SLAVE_TRACER))
    return false;

  // Inside tamp
  if(!getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::InsideTemp], ADDR_INSIDE_TEMP, MB_SLAVE_TRACER))
    return false;

  // Real-time status
  if(!getReadOnlyRegister(&CD_TracerParam.BatStat, ADDR_BAT_STATUS, MB_SLAVE_TRACER) ||
     !getReadOnlyRegister(&CD_TracerParam.CrgEquipStat, ADDR_CHRG_EQUIPMENT_STATUS, MB_SLAVE_TRACER) ||
     !getReadOnlyRegister(&CD_TracerParam.DisCrgEquipStat, ADDR_DISCHRG_EQUIPMENT_STATUS, MB_SLAVE_TRACER))
     return false;


  // Statistical data

  if(!getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::MaxVoltToday], ADDR_MAX_VOLT_TODAY, MB_SLAVE_TRACER) ||
  !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::MinVoltToday], ADDR_MIN_VOLT_TODAY, MB_SLAVE_TRACER) ||
  !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::MaxBatVoltToday], ADDR_MAX_BAT_VOLT_TODAY, MB_SLAVE_TRACER) ||
  !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::MinBatVoltToday], ADDR_MIN_BAT_VOLT_TODAY, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::ConsumedEnergyToday], ADDR_CONS_EN_TODAY_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::ConsumedEnergyMonth], ADDR_CONS_EN_MON_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::ConsumedEnergyYear], ADDR_CONS_EN_YEAR_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::ConsumedEnergyTotal], ADDR_CONS_EN_TOTAL_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::GeneratedEnergyToday], ADDR_GEN_EN_TODAY_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::GeneratedEnergyMonth], ADDR_GEN_EN_MON_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::GeneratedEnergyYear], ADDR_GEN_EN_YEAR_L, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::GeneratedEnergyTotal], ADDR_GEN_EN_TOTAL_L, MB_SLAVE_TRACER) ||
  !getValueSingleReg(&CD_TracerData[CD_TracerFloatDataType::BatVolt], ADDR_STAT_BAT_VOLT, MB_SLAVE_TRACER) ||
  !getValueDoubleReg(&CD_TracerData[CD_TracerFloatDataType::BatCur], ADDR_STAT_BAT_CURRENT_L, MB_SLAVE_TRACER))
  return false;
  
  return true;
}

