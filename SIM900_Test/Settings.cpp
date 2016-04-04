#include "Settings.h"
#include "String.h"
#include "stdio.h"
#include "EEPROM.h"
#include "modRtuCrcLib.h"

  
  
  void Settings_Class::init(void)
  {
    address = 02;
    period = 5; // minutes
    sprintf(phoneNum, "978784993");
    sprintf(email, "taraschrt06@gmail.com");
    sprintf(usrName, "arduino");
    sprintf(usrPwd, "arduino");
    sprintf(serverPath, "http://solar-test.comlu.com/storeData.php");

    sprintf(ap, "\"internet\"");
    sprintf(apUsr, "\"\"");
    sprintf(apPwd, "\"\"");

    restore((uint8_t*)phoneNum, SETT_PH_NUM_LEN, SETT_PH_NUM_ADDR);
    restore((uint8_t*)phoneNum, SETT_PH_NUM_LEN, SETT_PH_NUM_ADDR);
  }
  
  void Settings_Class::restoreAll(void)
  {
    int addr = SETT_PH_NUM_ADDR;
    int8_t cnt = 0;
    while(cnt < SETT_PH_NUM_LEN)
    {
      phoneNum[cnt] = EEPROM.read(addr++);
      cnt++;
    }
  }
  void Settings_Class::saveAll(void)
  {
    int addr = SETT_PH_NUM_ADDR;
    int8_t cnt = 0;
    while(cnt < SETT_PH_NUM_LEN)
    {
      EEPROM.write(addr++, phoneNum[cnt++]);
    }
  }

  void Settings_Class::restore(uint8_t* data, uint8_t len, int addr)
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
      
    }
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


