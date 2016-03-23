#ifndef _COMMON_DATA_H_
#define _COMMON_DATA_H_
#include "stdio.h"
#include "Settings.h"
#include "SIM900.h"
#include "Tracer.h"
#include "Pc.h"

#define CD_IO_BUF_SIZE  128

extern uint8_t ioBuf[CD_IO_BUF_SIZE];
extern Settings_Class Settings;
extern SIM900_Class sim900;
extern Tracer_Class tracer;
extern PC_Class pc;

/*** Tracer data ***/
enum CD_ChargingModes
{
  ConDisc = 0,
  Pwm,
  Mppt
};

enum CD_TracerFloatDataType
{
  /* Rated data 0x04 one reg */
  PvRatedVolt,
  PvRatedCur,
  PvRatedPow,
  BatRatedVolt,
  BatRatedCur,
  BatRatedPow,
  LoadRatedCur,
  
  /* Real time data 0x04 one reg */
  PvInVolt,
  PvInCur,
  PvInPow,
  BatPow,
  LoadVolt,
  LoadCur,
  LoadPow,
  BatTemp,
  InsideTemp,
  BatSoc,
  RemoteBatTemp,
  BatRealRatedVolt,

  /* Statistical data 0x04 one reg */
  MaxVoltToday,
  MinVoltToday,
  MaxBatVoltToday,
  MinBatVoltToday,
  ConsumedEnergyToday,
  ConsumedEnergyMonth,
  ConsumedEnergyYear,
  ConsumedEnergyTotal,
  GeneratedEnergyToday,
  GeneratedEnergyMonth,
  GeneratedEnergyYear,
  GeneratedEnergyTotal,
  BatVolt,
  BatCur,

  /* Holding registers 0x03 - read one, 0x10 - write multiple */
  
  /* Value to gat number of tracer data */
  TracerDataNum,
};

struct CD_TracerParams
{
  CD_ChargingModes ChargingMode; // 0 - connect/ disconnect, 1 - PWM, 2 - MPPT
  uint16_t BatStat;
  uint16_t CrgEquipStat;
  uint16_t DisCrgEquipStat;
};

extern struct CD_TracerParams CD_TracerParam;
/*** Tracer data end ***/

typedef enum
{
  AskTracer,
  AskNumber,
  AskBalans,
  SendDataGprs,
  
}CD_ProgramStates;

typedef struct
{
  CD_ProgramStates State;
  bool SendData;
}CD_CrType;
extern CD_CrType CD_Cr;


extern float CD_TracerData[CD_TracerFloatDataType::TracerDataNum];

/* variable that contains millis value */
/* Do not forget about interrupts before reseting timer!!! */
extern volatile unsigned long timer0_overflow_count;


#endif

