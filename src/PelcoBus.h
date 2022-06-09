/*!
 *  @file PelcoBus.h
 *
 *  @mainpage Manages all the bus communication with the Pelco cameras.
 *
 *  @section  Written by Boris Hilkens 05/06/2022
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */

#ifndef PELCOBUS_H
#define PELCOBUS_H

#include "Arduino.h"
#include <SoftwareSerial.h>

#include "constants.h"

class PelcoBus{

    private:
        SoftwareSerial* SerialCamBus;

        bool autoModule_;
                

        uint8_t messToCamera[7] = {
            0x00, // sync byte
            0x00, // Address
            0x00, // Always 0 (in most of the cases)
            0x00, // Command
            0x00, // Data1 (Used for pan speed and response from camera)
            0x00, // Data2 (Used everywhere)
            0x00  // Checksum: add all byte except sync and then modulo 0x100
        };

        uint8_t messFromcamera[7] = {
            0x00, // sync byte
            0x00, // Address
            0x00, // Always 0 (in most of the cases)
            0x00, // Command
            0x00, // Data1 (Used for pan speed and response from camera)
            0x00, // Data2 (Used everywhere)
            0x00  // Checksum: add all byte except sync and then modulo 0x100
        };

        uint8_t ACKmessFromCamera[4] = {
            0x00, //sync byte
            0x00, //Adress ?
            0x00, // 0?
            0x00  //Checksum taken for the previous message
        };

        int searchIndex(byte look_array[], byte value, size_t size);
        int searchIndexPROGMEM(const byte look_array[], byte value);

        uint16_t txPin_;
        uint16_t rxPin_;
        uint16_t rePin_;

        
bool log_messages_;
char log_buffer[100];

    public:

        PelcoBus(uint8_t txPin, uint8_t rxPin, uint8_t readEnPin = NOT_A_PIN);

        bool command(uint8_t address, bool disableACK , uint8_t command, uint16_t data1 = 0x00, uint8_t data2 = 0x00);
        uint16_t request(uint8_t address, uint8_t request, int timeout = 1000);

        void begin(uint32_t config, bool log_messages = false);

        bool send_raw(String hex_string); // TODO: get ACK
};

#endif