#include "Arduino.h"

// Include Libraries
#include <mcp_can.h>
#include "Parser.h"
#include <SPI.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// Instantiate CAN Connection
MCP_CAN _CAN(_CAN_PIN);

#define BACKLIGHT_PIN 3
#define I2C_ADDR 0x27
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

// Instantiate LCD connection
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);


/**
 * --------------------
 * Constructor
 * --------------------
 */
Parser::Parser()
{
    // Instantiated
}

/**
* --------------------
* Initialize
* --------------------
*/
void Parser::init()
{
    Serial.begin(115200);
    /**
     * --------------------
     * Variables
     * --------------------
     */

    // Public Attributes
    int rpm = 0;
    int speed = 0;

     // Private Attributes
    long unsigned int _rxId;
    unsigned char _len = 0;
    unsigned char _rxBuf[8];
    char _msgString[128];

    // Bus Data
    byte _data[8];

    // Run Setup
    Parser::_configureCAN();
    Parser::_configureLCD();
}


/**
* --------------------
* Configure the Can Connection
* --------------------
*/
void Parser::_configureCAN()
{
    // Initialize MCP2515 running at 16MHz with a baudrate of 10kb/s.
    if(_CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    {
        Serial.println("MCP2515 Initialized Successfully!");}
        else{
            Serial.println("Error Initializing MCP2515...");
        }

    // Change to normal mode to allow messages to be transmitted
    // Default is Loopback mode (debugging mode)
        _CAN.setMode(MCP_NORMAL);

    // Configure the an Input for receiving
        pinMode(_CAN_RECEIVE, INPUT);
    }

    void Parser::_configureLCD()
    {
        lcd.begin (16, 2);

    // Switch on the backlight
        lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
        lcd.setBacklight(HIGH);
    lcd.home (); // go home

    lcd.print("Spoiler");
    lcd.setCursor(0,1);
    lcd.print("Controller");
}

/**
 * --------------------
 * Update all values
 * --------------------
 */
void Parser::listen()
{
    // If _CAN_RECEIVE is low, read receive buffer
    if(!digitalRead(_CAN_RECEIVE))
    {
        // Read data: len = data length, buf = data byte(s)
        _CAN.readMsgBuf(&_rxId, &_len, _rxBuf);

        // Determine if ID is standard (11 bits) or extended (29 bits)
        if((_rxId & 0x80000000) == 0x80000000)
          sprintf(_msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (_rxId & 0x1FFFFFFF), _len);
      else
          sprintf(_msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", _rxId, _len);

        if((_rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
            sprintf(_msgString, " REMOTE REQUEST FRAME");
        } else {
            for(byte i = 0; i<_len; i++){
                sprintf(_msgString, "0x%.2X", _rxBuf[i]);
            }
        }
        Serial.println(_msgString);

        // Parse the incoming frame
        // and update the public attribute
        Parser::_parse();
    }
}

/**
 * --------------------
 * Parse the current request frame
 * --------------------
 */
void Parser::_parse()
{
    if(_rxId == 12){
        // RPM
        rpm =((_rxBuf[0] * 256) + _rxBuf[1])/4;

        lcd.setCursor(0,0);
        lcd.print("RPM: ");
        lcd.print(rpm);
    } else if(_rxId == 13){
        // Speed
        speed =_rxBuf[0];

        lcd.setCursor(0,1);
        lcd.print("Speed: ");
        lcd.print(speed);
    }
}
