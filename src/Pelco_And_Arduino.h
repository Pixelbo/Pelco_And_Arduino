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

#include "constants.h"

class PelcoCam
{

private:
    SoftwareSerial* SerialCam;

    uint8_t address_;
    uint32_t config_;

    uint16_t txPin_;
    uint16_t rxPin_;
    uint16_t rePin_;

    bool log_messages_;
    bool autoModule_;

    char log_buffer[200];

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

    int searchIndex(const byte look_array[], byte value);

public:
    PelcoCam(uint8_t address, uint32_t config, uint8_t txPin, uint8_t rxPin, bool log_messages = false, uint8_t readEnPin = NOT_A_PIN);
    void begin();

    bool send_command(uint8_t command, uint16_t data1 = 0x00, uint8_t data2 = 0x00, bool request = false);
    uint16_t send_request(uint8_t request, int timeout = 1000, int max_buffer = 20);
    bool send_raw(String hex_string); // TODO: get ACK
};

#endif