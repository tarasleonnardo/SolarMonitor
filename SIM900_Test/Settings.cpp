#include "Settings.h"
#include "String.h"
#include "stdio.h"
#include "EEPROM.h"
#include "modRtuCrcLib.h"

  
  
  void Settings_Class::init(void)
  {
    address = 0x02;
    restoreAll();
  }
  
  void Settings_Class::restoreAll(void)
  {
    uint16_t per[2];
    restore((uint8_t*)per, SETT_PERIOD_LEN, SETT_PERIOD_ADDRESS);
    memcpy(&timeout, per, 2);

    restore((uint8_t*)phoneNum, SETT_PH_NUM_LEN, SETT_PH_NUM_ADDR);
    restore((uint8_t*)email, SETT_EMAIL_LEN, SETT_EMAIL_ADDR);
    restore((uint8_t*)usrName, SETT_USR_NAME_LEN, SETT_USR_NAME_ADDR);
    restore((uint8_t*)usrPwd, SETT_USR_PWD_LEN, SETT_USR_PWD_ADDR);
    restore((uint8_t*)ap, SETT_AP_NAME_LEN, SETT_AP_NAME_ADDR);
    restore((uint8_t*)apUsr, SETT_AP_USR_LEN, SETT_AP_USR_ADDR);
    restore((uint8_t*)apPwd, SETT_AP_PWD_LEN, SETT_AP_PWD_ADDR);
    restore((uint8_t*)serverPath, SETT_SERVER_PATH_LEN, SETT_SERVER_PATH_ADDR);
  }
  void Settings_Class::saveAll(void)
  {
    save((uint8_t*)timeout, SETT_PERIOD_LEN, SETT_PERIOD_ADDRESS);
    save((uint8_t*)phoneNum, SETT_PH_NUM_LEN, SETT_PH_NUM_ADDR);
    save((uint8_t*)email, SETT_EMAIL_LEN, SETT_EMAIL_ADDR);
    save((uint8_t*)usrName, SETT_USR_NAME_LEN, SETT_USR_NAME_ADDR);
    save((uint8_t*)usrPwd, SETT_USR_PWD_LEN, SETT_USR_PWD_ADDR);
    save((uint8_t*)ap, SETT_AP_NAME_LEN, SETT_AP_NAME_ADDR);
    save((uint8_t*)apUsr, SETT_AP_USR_LEN, SETT_AP_USR_ADDR);
    save((uint8_t*)apPwd, SETT_AP_PWD_LEN, SETT_AP_PWD_ADDR);
    save((uint8_t*)serverPath, SETT_SERVER_PATH_LEN, SETT_SERVER_PATH_ADDR);

    //http://solar-test.comlu.com/storeData.php
  }

  bool Settings_Class::restore(uint8_t* data, uint8_t len, int addr)
  {
    uint16_t crc;
    while(len--)
    {
      *data = EEPROM.read(addr++);
      data++;
    }

    crc = EEPROM.read(addr++);
    crc |= (EEPROM.read(addr++) << 8);

    if(CRC_Ok != CRC_ModRtuCrcCheck(crc, data, len))
    {
      return false;
    }

    return true;
  }
  void Settings_Class::save(uint8_t* data, uint8_t len, int addr)
  {
    uint16_t crc = CRC_ModRtuCrcCalc(data, len);
    while(len--)
    {
      EEPROM.write(addr++, *data);
      data++;
    }

    EEPROM.write(addr++, crc & 0xFF); // Low crc byte
    EEPROM.write(addr++, (crc >> 8) & 0xFF); // High crc byte
  }


