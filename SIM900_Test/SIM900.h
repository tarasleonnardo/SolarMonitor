#ifndef _SIM900_CLASS_H_
#define _SIM900_CLASS_H_
class SIM900_Class
{
  public:
  typedef enum
  {
    SIM900_NO_ERR = 0,
    SIM900_NO_ANS = 1,
    
  }ErrorsType;
  
  void init(void);
  void powerSwitch(void);
  void startListening(void);
  bool isOn(void); // AT - OK check
  bool checkAccount(void);
  bool getNumber(void);
  ErrorsType sendRealTimeData(void);

  private:
  float balans;
  char buf[256] = {0};

  bool checkOk(void);

};
#endif

