#ifndef _TRACER_H_
#define _TRACER_H_

#include "stdint.h"
#include <SoftwareSerial.h>

#define TRACER_BUF_SIZE 32U
//#define TRACER_DEBUG // pring debug info

#define MB_SLAVE_ADDR   0x01  // Modbus address of tracer
#define TRACER_TIMES    100.f

class Tracer_Class
{
  public:
   typedef enum
   {
    ADDR_PV_RATED_VOLTAGE = 0x3000,
    ADDR_PV_RATED_CURRENT = 0x3001,
    ADDR_PV_RATED_POW_L = 0x3002,
    ADDR_PV_RATED_POW_H = 0x3003,

    ADDR_BAT_VOLTAGE = 0x3004,
    ADDR_BAT_CURRENT = 0x3005,
    ADDR_BAT_POWER_L = 0x3006,
    ADDR_BAT_POWER_H = 0x3007,

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

    ADDR_BAT_RE_RATED_POW = 0x311D,

    ADDR_BAT_STATUS = 0x3200,
    ADDR_CHRG_EQUIPMENT_STATUS = 0x3201,
    ADDR_DISCHRG_EQUIPMENT_STATUS = 0x3202,

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
  bool getRegister(uint16_t* reg, REGISTERS_ADDRESSES regN);
  // Refresh all data in groups
  bool refreshRatedData(void);
  bool refreshRealTimeData(void);
  private:
  void send(void);
  bool receive(void);
  void formAskFrame(REGISTERS_ADDRESSES val, uint16_t num);

  // data
  uint8_t lastCmd;
  uint8_t bufCount;
  uint8_t buf[TRACER_BUF_SIZE];


  public:
  // Tracer data
  struct
  {
    float ArrayRatedVolt;
    float ArrayRatedCur;
    float ArrayRatedPow;
    float BatRatedVolt;
    float BatRatedCurr;
    float BatRatedPow;
    float LoadRatedCurr;
    uint16_t CrgMode;
  }RatedData;

  struct
  {
    float ArrayInVolt;
    float ArrayInCur;
    float ArrayInPow;
    float BatPow;
    float LoadInVolt;
    float LoadInCur;
    float LoadInPow;
    float BatTemp;
    float InsideTemp;
    float BatSoc;
    float RemoteBatTemp;
    float BatRealRatedPow;
    uint16_t BatStatus;
    uint16_t ChrgEquipmentStatus;
    uint16_t DischargingEquipmentStatus;
  }RealTimeData;
  

  
};

#endif

