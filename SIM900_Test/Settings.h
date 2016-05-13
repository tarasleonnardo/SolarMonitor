#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdint.h"

#define SETT_CRC_SIZE   2
#define SETT_MODBUS_ADDR_LEN  1
#define SETT_PERIOD_LEN 2
#define SETT_PH_NUM_LEN   10
#define SETT_EMAIL_LEN    32
#define SETT_USR_NAME_LEN 16
#define SETT_USR_PWD_LEN  32
#define SETT_AP_NAME_LEN  16
#define SETT_AP_USR_LEN  5
#define SETT_AP_PWD_LEN  5
#define SETT_SERVER_PATH_LEN  100


#define SETT_MODBUS_ADDR_ADDR 0
#define SETT_PERIOD_ADDRESS  (SETT_MODBUS_ADDR_ADDR + SETT_MODBUS_ADDR_LEN + SETT_CRC_SIZE)
#define SETT_PH_NUM_ADDR     (SETT_PERIOD_ADDRESS + SETT_PERIOD_LEN + SETT_CRC_SIZE)
#define SETT_EMAIL_ADDR      (SETT_PH_NUM_ADDR +  SETT_PH_NUM_LEN + SETT_CRC_SIZE)
#define SETT_USR_NAME_ADDR   (SETT_EMAIL_ADDR + SETT_EMAIL_LEN + SETT_CRC_SIZE)
#define SETT_USR_PWD_ADDR    (SETT_USR_NAME_ADDR + SETT_USR_NAME_LEN + SETT_CRC_SIZE)
#define SETT_AP_NAME_ADDR    (SETT_USR_PWD_ADDR + SETT_USR_PWD_LEN + SETT_CRC_SIZE)
#define SETT_AP_USR_ADDR     (SETT_AP_NAME_ADDR + SETT_AP_NAME_LEN + SETT_CRC_SIZE)
#define SETT_AP_PWD_ADDR     (SETT_AP_USR_ADDR + SETT_AP_USR_LEN + SETT_CRC_SIZE)
#define SETT_SERVER_PATH_ADDR  (SETT_AP_PWD_ADDR + SETT_AP_PWD_LEN + SETT_CRC_SIZE)

class Settings_Class
{
  public: // methods
  void init(void);
  void restoreAll(void);
  void saveAll(void);
  bool restore(uint8_t* data, uint8_t len, int addr);
  void save(uint8_t* data, uint8_t len, int addr);

  public: // data
  uint8_t address;
  uint16_t timeout; // sec

  // strings
  char phoneNum[SETT_PH_NUM_LEN];
  char email[SETT_EMAIL_LEN];
  char usrName[SETT_USR_NAME_LEN];
  char usrPwd[SETT_USR_PWD_LEN];
  char ap[SETT_AP_NAME_LEN];
  char apUsr[SETT_AP_USR_LEN];
  char apPwd[SETT_AP_PWD_LEN];

  char serverPath[SETT_SERVER_PATH_LEN];
};

#endif

