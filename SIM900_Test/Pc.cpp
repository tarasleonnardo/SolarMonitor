#include "Pc.h"
#include "Tracer.h"
#include "Settings.h"
#include "CommonData.h"

#include "Arduino.h"
#include "stdio.h"
#include "string.h"
#include "modRtuCrcLib.h"

void PC_Class::init(void)
{
 Serial.begin(9600);
 discCnt = 0;
 Serial.println("PC protocol started");
}

bool PC_Class::getCmd(void)
{
  uint16_t crc;
  char buf[32];

  if(pc.connected)
  {
    sprintf((char*)ioBuf, "discCnt = %d", discCnt);
    Serial.println((char*)ioBuf);
  } 
  if(pc.connected  && (++ discCnt >= 50))
  {
    pc.connected = false;
    discCnt = 0;
  }
  
  while(0 < Serial.available())
  {
    if(Settings.address != (ioBuf[PC_B_ADDR] = Serial.read()))
    {
      continue;
    }
    if(3 != Serial.readBytes(ioBuf + 1, 3))
    {
      continue;
    }
    if((ioBuf[PC_B_DATA_NUM] + 2) != Serial.readBytes(ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM] + 2))
    {
      Serial.write(ioBuf, 8);
      return false;
    }
    crc = ioBuf[PC_B_CRC_L];
    crc |= (ioBuf[PC_B_CRC_H] << 8);
    
    if(CRC_Ok == CRC_ModRtuCrcCheck(crc, ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES))
    {
      selectCommand();
      return true;
    }
    sendAns(PC_AnsNotOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
    return false;
  }
  return false;
}

void PC_Class::selectCommand(void)
{
  if(!connected)
  {
    CD_Cr.SendData = false;
  }
  connected = true;
  discCnt = 0;
  
  switch(ioBuf[PC_B_CMD])
  {
    case PC_CmdIsAlive:
    sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
    break;
    case PC_GetPhoneNum:
      if(!ioBuf[PC_B_RW]) sendString(Settings.phoneNum, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.phoneNum, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.phoneNum[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.phoneNum, SETT_PH_NUM_LEN, SETT_PH_NUM_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetEmail:
      if(!ioBuf[PC_B_RW]) sendString(Settings.email, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.email, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.email[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.email, SETT_EMAIL_LEN, SETT_EMAIL_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetUsrName:
      if(!ioBuf[PC_B_RW]) sendString(Settings.usrName, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.usrName, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.usrName[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.usrName, SETT_USR_NAME_LEN, SETT_USR_NAME_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetUsrPwd:
      if(!ioBuf[PC_B_RW]) sendString(Settings.usrPwd, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.usrPwd, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.usrPwd[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.usrPwd, SETT_USR_PWD_LEN, SETT_USR_PWD_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetAp:
      if(!ioBuf[PC_B_RW]) sendString(Settings.ap, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.ap, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.ap[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.ap, SETT_AP_NAME_LEN, SETT_AP_NAME_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetApUsr:
      if(!ioBuf[PC_B_RW]) sendString(Settings.apUsr, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.apUsr, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.apUsr[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.apUsr, SETT_AP_USR_LEN, SETT_AP_USR_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetApPwd:
      if(!ioBuf[PC_B_RW]) sendString(Settings.apPwd, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        memcpy((void*)Settings.apPwd, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.apPwd[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.apPwd, SETT_AP_PWD_LEN, SETT_AP_PWD_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetServerPath:
      if(!ioBuf[PC_B_RW]) sendString(Settings.serverPath, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      else
      {
        if(ioBuf[PC_B_DATA_NUM] > SETT_SERVER_PATH_LEN)  ioBuf[PC_B_DATA_NUM] = SETT_SERVER_PATH_LEN;
        memcpy((void*)Settings.serverPath, (void*)ioBuf + PC_B_DATA_START, ioBuf[PC_B_DATA_NUM]);
        Settings.serverPath[ioBuf[PC_B_DATA_NUM]] = '\0';
        Settings.save((uint8_t *)Settings.serverPath, SETT_SERVER_PATH_LEN, SETT_SERVER_PATH_ADDR);
        sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
    break;
    case PC_GetTimeout:
      if(!ioBuf[PC_B_RW])
      {
        sprintf((char*)ioBuf + 20, "%d", Settings.timeout);
        sendString((char*)ioBuf + 20, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
      }
      else
      {
        if(isdigit(ioBuf[PC_B_DATA_START]) && ioBuf[PC_B_DATA_START] > 0x30)
        {
          Settings.timeout = atoi((char*)ioBuf + PC_B_DATA_START);
          if(Settings.timeout < 15) Settings.timeout = 15;
          Settings.save((uint8_t *)&Settings.timeout, SETT_PERIOD_LEN, SETT_PERIOD_ADDRESS);
          sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
        }
      }
    break;
    /* Tracer */
    case PC_CmdGetTracerRatedData:
      sendTracerRatedData();
    break;
    case PC_CmdGetTracerRealTimeData:
      sendTracerRealTimeData();
    break;
    case PC_SendData:
      CD_Cr.SendData = true;
      sendAns(PC_AnsOk, (PC_Class::PC_Commands)ioBuf[PC_B_CMD]);
    break;
    default:
    Serial.println("unknown cmd");
    break;
  }
}


void PC_Class::sendAns(PC_AnsType ans, PC_Commands cmd)
{
  uint16_t crc;
  
  ioBuf[0] = Settings.address;
  ioBuf[1] = cmd;
  ioBuf[2] = 0; // RW
  ioBuf[3] = 1; // one data byte
  ioBuf[4] = ans;

  crc = CRC_ModRtuCrcCalc(ioBuf, 5);

  ioBuf[PC_B_CRC_L] = crc & 0xFF;
  ioBuf[PC_B_CRC_H] = (crc >> 8) & 0xFF;

  Serial.write(ioBuf, 7);
}
void PC_Class::sendTracerData(void)
{
  
}
void PC_Class::sendSettigs(void)
{
  
}

void PC_Class::sendString(char* str, PC_Commands cmd)
{
  uint16_t crc;
  
  ioBuf[0] = Settings.address;
  ioBuf[1] = cmd;
  ioBuf[2] = 0; // RW
  ioBuf[3] = strlen(str) + 1;

  strcpy((char*)ioBuf + PC_B_DATA_START, str);
  
  crc = CRC_ModRtuCrcCalc(ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES);

  ioBuf[PC_B_CRC_L] = crc & 0xFF;
  ioBuf[PC_B_CRC_H] = (crc >> 8) & 0xFF;

  Serial.write(ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES + 2);
}

void PC_Class::sendTracerRatedData(void)
{
  uint16_t crc = 4;
  
  ioBuf[0] = Settings.address;
  ioBuf[1] = PC_CmdGetTracerRatedData;
  ioBuf[2] = 0; // RW
  ioBuf[3] = 30;

  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::PvRatedVolt], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::PvRatedCur], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::PvRatedPow], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatRatedVolt], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatRatedCur], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatRatedPow], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::LoadRatedCur], sizeof(float));
  crc+= sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerParam.ChargingMode, sizeof(CD_ChargingModes));
    
  crc = CRC_ModRtuCrcCalc(ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES);

  ioBuf[PC_B_CRC_L] = crc & 0xFF;
  ioBuf[PC_B_CRC_H] = (crc >> 8) & 0xFF;

  Serial.write(ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES + 2);
}

void PC_Class::sendTracerRealTimeData(void)
{
  uint16_t crc = 4;
  
  ioBuf[0] = Settings.address;
  ioBuf[1] = PC_CmdGetTracerRealTimeData;
  ioBuf[2] = 0; // RW
  ioBuf[3] = 54;

  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::PvInVolt], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::PvInCur], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::PvInPow], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatPow], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::LoadVolt], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::LoadCur], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::LoadPow], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatTemp], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::InsideTemp], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatSoc], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::RemoteBatTemp], sizeof(float));
  crc += sizeof(float);
  memcpy(ioBuf + crc, &CD_TracerData[CD_TracerFloatDataType::BatRealRatedVolt], sizeof(float));
  crc += sizeof(float);
  
  memcpy(ioBuf + crc, &CD_TracerParam.BatStat, sizeof(CD_TracerParam.BatStat));
  crc += sizeof(uint16_t);
  memcpy(ioBuf + crc, &CD_TracerParam.CrgEquipStat, sizeof(CD_TracerParam.CrgEquipStat));
  crc += sizeof(uint16_t);
  memcpy(ioBuf + crc, &CD_TracerParam.DisCrgEquipStat, sizeof(CD_TracerParam.DisCrgEquipStat));
  crc += sizeof(uint16_t);

  
  crc = CRC_ModRtuCrcCalc(ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES);

  ioBuf[PC_B_CRC_L] = crc & 0xFF;
  ioBuf[PC_B_CRC_H] = (crc >> 8) & 0xFF;

  Serial.write(ioBuf, ioBuf[PC_B_DATA_NUM] + PC_EXTRA_BYTES + 2);
}



