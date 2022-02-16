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
#define PAN_L  0x04
#define PAN_R  0x02
#define TILT_U  0x08
#define TILT_D  0x10

#define PAN_L_TILT_U  0x0C
#define PAN_R_TILT_U  0x0A
#define PAN_L_TILT_D  0x14
#define PAN_R_TILT_D  0x12

#define ZOOM_T  0x20
#define ZOOM_W  0x40
#define SET_ZOOM_SPEED  0x25

#define STOP  0x00

#define ON  0x88 //!!!!not byte 4 but 3
#define OFF  0x08

#define RESET  0x29

#define SET_PRESET  0x03 //data2 preset id
#define GOTO_PRESET  0x05 //data2 preset id
#define CLR_PRESET  0x07 //data2 preset id

#define QUERY_PAN  0x51
#define QUERY_TILT  0x53
#define QUERY_ZOOM  0x55
#define RESP_PAN  0x59
#define RESP_TILT  0x5B
#define RESP_ZOOM  0x5D

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


        void logMessage(uint8_t hexString[]);

    public:
        PelcoCam(uint8_t Address, int baud, int txPin, int rxPin, bool log_messages);
        void send_message(uint8_t command, uint8_t params, uint8_t params2);

    
};

#endif