#ifndef _SIM900_CLASS_H_
#define _SIM900_CLASS_H_

#include "stdint.h"

class SIM900_Class
{
  public:
  typedef enum
  {
    SIM900_NO_ERR = 0,
    SIM900_NO_ANS = 1,
    
  }ErrorsType;
  
  void init(void);
  void listen(void);
  bool turnPower(bool on);
  bool isOn(void); // AT - OK check
  bool checkAccount(void);
  bool getNumber(void);
  bool getSigLevel(void);
  ErrorsType sendRealTimeData(void);

  public:
  void discardBuffer(void);
  bool checkAns(char* ans, unsigned long timeout);
  void writeGetStart(void);
  bool writeRatedData(void);
  bool writeRealTimeData(void);
  bool writeStatisticData(void);
  bool writeGsmData(void);
  
  float balans;
  float signalLevel;
  char phoneNum[14];
  char buf[256] = {0};

  bool checkOk(void);

};
#endif

