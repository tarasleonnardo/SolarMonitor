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

/* variable that contains millis value */
/* Do not forget about interrupts before reseting timer!!! */
extern volatile unsigned long timer0_overflow_count;


#endif

