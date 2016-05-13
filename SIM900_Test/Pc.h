#ifndef _PC_H_
#define _PC_H_

#include "stdint.h"

#define PC_B_ADDR       0x00
#define PC_B_CMD        0x01
#define PC_B_RW         0x02
#define PC_B_DATA_NUM   0x03
#define PC_B_DATA_START 0x04
#define PC_EXTRA_BYTES  4

#define PC_B_CRC_L      (ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES + 1)
#define PC_B_CRC_H      (ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES)

class PC_Class
{
  public:
  void init(void);
  bool getCmd(void);

  bool connected = false;
  uint8_t discCnt = 0;

  private:
  typedef enum
  {
    PC_CmdIsAlive = 0,
    /*** Settings ***/
    PC_GetPhoneNum,
    PC_GetEmail,
    PC_GetUsrName,
    PC_GetUsrPwd,
    PC_GetAp,
    PC_GetApUsr,
    PC_GetApPwd,
    PC_GetServerPath,
    PC_GetTimeout,

    /** Tracer **/
    PC_CmdGetTracerRatedData,
    PC_CmdGetTracerRealTimeData,

    PC_SendData,
    PC_SendString,
  }PC_Commands;

  typedef enum
  {
    PC_AnsOk = 0,
    PC_AnsNotOk
  }PC_AnsType;

  void selectCommand(void);
  void sendAns(PC_AnsType ans, PC_Commands cmd);
  void sendTracerData(void);
  void sendSettigs(void);
  void sendString(char* str, PC_Commands cm);
  void sendTracerRatedData(void);
  void sendTracerRealTimeData(void);
};

#endif

