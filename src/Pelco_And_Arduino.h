/*!
 *  @file Pelco_And_Arduino.h
 *
 *  @mainpage The root of this libray!
 *
 *  @section author Written by Boris Hilkens 15/02/2022.
 * 
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */


#ifndef Pelco_And_Arduino_H
#define Pelco_And_Arduino_H

#include "Arduino.h"
#include <SoftwareSerial.h>

//////////////////////////////////////////Some constants
const uint8_t PROGMEM PAN_L = 0x04;
const uint8_t PROGMEM PAN_R = 0x02;
const uint8_t PROGMEM TILT_U = 0x08;
const uint8_t PROGMEM TILT_D = 0x10;

const uint8_t PROGMEM PAN_L_TILT_U = 0x0C;
const uint8_t PROGMEM PAN_R_TILT_U = 0x0A;
const uint8_t PROGMEM PAN_L_TILT_D = 0x14;
const uint8_t PROGMEM PAN_R_TILT_D = 0x12;

const uint8_t PROGMEM ZOOM_T = 0x20;
const uint8_t PROGMEM ZOOM_W = 0x40;
const uint8_t PROGMEM SET_ZOOM_SPEED = 0x25;

const uint8_t PROGMEM STOP = 0x00;

const uint8_t PROGMEM ON = 0x88; //!!!!not byte 4 but 3
const uint8_t PROGMEM OFF = 0x08;

const uint8_t PROGMEM RESET = 0x29;

const uint8_t PROGMEM SET_PRESET = 0x03; //data2 preset id
const uint8_t PROGMEM GOTO_PRESET = 0x05; //data2 preset id
const uint8_t PROGMEM CLR_PRESET = 0x07; //data2 preset id

const uint8_t PROGMEM QUERY_PAN = 0x51;
const uint8_t PROGMEM QUERY_TILT = 0x53;
const uint8_t PROGMEM QUERY_ZOOM = 0x55;
const uint8_t PROGMEM RESP_PAN = 0x59;
const uint8_t PROGMEM RESP_TILT = 0x5B;
const uint8_t PROGMEM RESP_ZOOM = 0x5D;

/*

A lot of commands is not implemented yet, I'll do it in the future!
 */

/////////////////////////////////////////////////

class PelcoCam {
    private:
        SoftwareSerial SerialCam;

        uint8_t Address_;
        uint32_t baud_;
        uint16_t txPin_;
        uint16_t rxPin_;
        bool log_messages_;

        uint8_t messToCamera[7] ={
                       0x00, //sync byte
                       0x00, //Address
                       0x00, //Always 0 (in most of the cases)
                       0x00, //Command
                       0x00, //Data1 (Used for pan speed and response from camera)
                       0x00, //Data2 (Used everywhere)
                       0x00  //Checksum: add all byte except sync and then modulo 0x100
                       };

        uint8_t messFromcamera[7]={
                         0x00, //sync byte
                         0x00, //Address
                         0x00, //Always 0 (in most of the cases)
                         0x00, //Command
                         0x00, //Data1 in MSB
                         0x00, //Data2 in LSB
                         0x00  //Checksum: add all byte except sync and then modulo 0x100
                         };

    public:
        PelcoCam(uint8_t Address, int baud, int txPin, int rxPin, bool log_messages);
        void begin();
        void send_message(uint8_t command, uint8_t params=0x00, uint8_t params2=0x00);

    
};

#endif