/*
  Parser.h - Library for parsing OBD2 data for a Citroën C3 2002
*/
#ifndef Parser_h
#define Parser_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif


class Parser{
  public:
    int rpm;
    int speed;

    Parser(); // Constructor
    void init();
    void listen(); // Listen for Data

  private:
    long unsigned int _rxId;
    unsigned char _len = 0;
    unsigned char _rxBuf[8];
    char _msgString[128];

    byte _data[8];
    byte _sentMessage;

    #define _CAN_RECEIVE 2
    #define _CAN_PIN 10

    void _configureCAN();
    void _parse();
};

#endif
