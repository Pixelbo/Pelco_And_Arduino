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

class PelcoBus
{

private:
    SoftwareSerial* SerialCamBus;

    uint16_t txPin_;
    uint16_t rxPin_;
    uint16_t rePin_;

    bool support_rx_;
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

    int searchIndex(byte look_array[], byte value, size_t size);
    int searchIndexPROGMEM(const byte look_array[], byte value);

public:
    PelcoBus(uint8_t txPin, uint8_t rxPin, uint8_t readEnPin = NOT_A_PIN);

    struct PelcoCam{
        uint8_t address;
        bool disable_ack;
    };
    

    void begin(uint32_t config, bool log_messages = false);

    bool send_command(PelcoBus::PelcoCam camera, uint8_t command, uint16_t data1 = 0x00, uint8_t data2 = 0x00, bool disableACK = false);
    uint16_t send_request(PelcoBus::PelcoCam camera, uint8_t request, int timeout = 1000);
    bool send_raw(String hex_string); // TODO: get ACK
};

#endif