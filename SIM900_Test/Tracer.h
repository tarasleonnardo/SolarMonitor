#ifndef _TRACER_H_
#define _TRACER_H_

#include "stdint.h"
#include <SoftwareSerial.h>

//#define TRACER_DEBUG // pring debug info

#define MB_SLAVE_ADDR   0x01  // Modbus address of tracer
#define TRACER_TIMES    100.f

class Tracer_Class
{
public:
   typedef enum
   {

    // Read only registers
    ADDR_PV_RATED_VOLTAGE = 0x3000,
    ADDR_PV_RATED_CURRENT = 0x3001,
    ADDR_PV_RATED_POW_L = 0x3002,
    ADDR_PV_RATED_POW_H = 0x3003,

    ADDR_BAT_RATED_VOLTAGE = 0x3004,
    ADDR_BAT_RATED_CURRENT = 0x3005,
    ADDR_BAT_RATED_POWER_L = 0x3006,
    ADDR_BAT_RATED_POWER_H = 0x3007,

    ADDR_CHARGING_MODE  = 0x3008,

    ADDR_LOAD_RATED_CURRENT = 0x300E,
    
    ADDR_INPUT_VOLTAGE = 0x3100,
    ADDR_INPUT_CURRENT = 0x3101,
    ADDR_INPUT_POWER_L = 0x3102,
    ADDR_INPUT_POWER_H = 0x3103,
    
    ADDR_BAT_CHRG_VOLTAGE = 0x3104,
    ADDR_BAT_CHRG_CURRENT = 0x3105,
    ADDR_BAT_CHRG_POWER_L = 0x3106,
    ADDR_BAT_CHRG_POWER_H = 0x3107,
    
    ADDR_LOAD_VOLTAGE = 0x310C,
    ADDR_LOAD_CURRENT = 0x310D,
    ADDR_LOAD_POWER_L = 0x310E,
    ADDR_LOAD_POWER_H = 0x310F,

    ADDR_BAT_TEMP = 0x3110,
    ADDR_INSIDE_TEMP = 0x3111,
    ADDR_HS_TEMP = 0x3112,
    ADDR_BAT_SOC = 0x311A,
    ADDR_BAT_REMOTE_TEMP = 0x311B,

    ADDR_BAT_RE_RATED_VOLT = 0x311D,

    ADDR_BAT_STATUS = 0x3200,
    ADDR_CHRG_EQUIPMENT_STATUS = 0x3201,
    ADDR_DISCHRG_EQUIPMENT_STATUS = 0x3202,

  // Statistical data
    ADDR_MAX_VOLT_TODAY = 0x3300,
    ADDR_MIN_VOLT_TODAY = 0x3301,
    ADDR_MAX_BAT_VOLT_TODAY = 0x3302,
    ADDR_MIN_BAT_VOLT_TODAY = 0x3303,

    ADDR_CONS_EN_TODAY_L = 0x3304,
    ADDR_CONS_EN_MON_L = 0x3306,
    ADDR_CONS_EN_YEAR_L = 0x3308,
    ADDR_CONS_EN_TOTAL_L = 0x330A,

    ADDR_GEN_EN_TODAY_L = 0x330C,
    ADDR_GEN_EN_MON_L = 0x330E,
    ADDR_GEN_EN_YEAR_L = 0x3310,
    ADDR_GEN_EN_TOTAL_L = 0x3312,

    ADDR_STAT_BAT_VOLT = 0x331A,
    ADDR_STAT_BAT_CURRENT_L = 0x331B,
     
    ADDR_OVR_TEMP_INSIDE = 0x2000
   }REGISTERS_ADDRESSES;
private:
   typedef enum
  {
    MB_CMD_READ_FLAGS_REGS = 0x01,
    MB_CMD_READ_DIG_INPUTS = 0x02,
    MB_CMD_READ_STORE_REGS = 0x03,
    MB_CMD_READ_INPUT_REGS = 0x04,

    MB_CMD_WRITE_ONE_FLAG = 0x05,
    MB_CMD_WRITE_ONE_STORE_REG = 0x06,

    MB_CMD_WRITE_FEW_FLAGS_REGS = 0x0F,
    MB_CMD_WRITE_FEW_STORE_REGS = 0x10
  }MODBUS_CMDS;
  
  public:
  void init(void);
  void startListening(void);
  // Get single register
  bool getValueDoubleReg(float* val, REGISTERS_ADDRESSES regnL);
  bool getValueSingleReg(float* val, REGISTERS_ADDRESSES regN);
  bool getReadOnlyRegister(uint16_t* reg, REGISTERS_ADDRESSES regN);
  bool getRwRegister(uint16_t* reg, REGISTERS_ADDRESSES regN);
  // Refresh all data in groups
  bool refreshRatedData(void);
  bool refreshReadOnlyData(void);
  private:
  void send(void);
  bool receive(void);
  void formAskFrame(REGISTERS_ADDRESSES val, MODBUS_CMDS cmd, uint16_t num);

  // data
  uint8_t lastCmd;
  uint8_t bufCount;  
};

#endif

