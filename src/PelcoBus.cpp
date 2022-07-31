/*!
 *  @file PelcoBus.cpp
 *
 *  @mainpage Manages all the bus communication with the Pelco cameras.
 *
 *  @section  Written by Boris Hilkens 05/06/2022
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include <SoftwareSerial.h>

#include "Arduino.h"

#include "PelcoBus.h"
#include "utils.h"

/*!
 *  @brief  Constructor
 *
 *  @param  rxPin the RX pin of the module (Cam to Arduino)
 *  @param  txPin the TX pin of the module (Arduino to Cam)
 *  @param  readEnPin Pin for RS485 modules that require toggle between rx and
 *                      tx   (RE and \DE pin of the module)
 *
 */

PelcoBus::PelcoBus(uint8_t rxPin, uint8_t txPin, int8_t readEnPin) {
    txPin_ = txPin;
    rxPin_ = rxPin;
    rePin_ = readEnPin;
      
}

/*!
 *  @brief  begin everything and check some things
 *
 *  @param config You lust specify the baud here, see constants for all possibles configs
 *  @param log_messages If true, outouts debug messages to the defaut serial output
 */

void PelcoBus::begin(uint32_t config, bool log_messages) {

    uint16_t baud;

    switch (config) {
    case PELCO_D2400:
        baud = 2400;
        break;
    case PELCO_D4800:
        baud = 4800;
        break;
    case PELCO_D9600:
        baud = 9600;
        break;
    }
   
    utils.log_messages_ = false;

    if (log_messages) {
        if (!Serial) {
            Serial.begin(9600);
        }

        Serial.print(F("Message log has been activated for the Pelco Bus!\n"));

        sprintf_P(utils.log_buffer, (const char *)F("PelcoBus config: baud=%u and protocol D\n"), baud);
        Serial.print(utils.log_buffer);

        utils.log_messages_ = true;
    }

    // Declare pins
    pinMode(rxPin_, INPUT);
    pinMode(txPin_, OUTPUT);

    // Is the module an auto switching between tx and RX ?
    autoModule_ = true;

    if (rePin_ != -1) {
        pinMode(rePin_, OUTPUT);
        autoModule_ = false;

        digitalWrite(rePin_, HIGH); // Set the module in tx mode
    }

    // Initialize the serial bus
    SerialCamBus = new SoftwareSerial(rxPin_, txPin_);
    (*SerialCamBus).begin(baud);
}

/*!
 *  @brief  Send message to the camera
 *
 *  @param  address Address of the camera
 *  @param  disableACK Disable ACK, useful for request and for old cameras
 *  @param  command the wanted command to pass
 *  @param  data1 Main parameter
 *  @param  data2 Second parameter for command that requires 2 parameters
 *  @return true if succeed, false if not succeed
 *
 */

bool PelcoBus::command(uint8_t address, bool disableACK, uint8_t command, uint16_t data1, uint8_t data2) {
    messToCamera[0] = 0xFF;    // The first byte is always FF (sync)
    messToCamera[1] = address; // the second is the adress

    // Special commands handling:
    if (utils.searchIndexPROGMEM(CMND1, command) != -1) {
        messToCamera[2] = command - 0b01100000; // Minus the thing that differentiate from others
        messToCamera[3] = 0x00;
        messToCamera[4] = 0x00;
        messToCamera[5] = 0x00;

    } else { // that means command is the byte 4
        messToCamera[2] = 0x00;
        messToCamera[3] = command;

        if (utils.searchIndexPROGMEM(DATA1, command) != -1) {
            messToCamera[4] = data1;
            messToCamera[5] = 0x00;

        } else if (utils.searchIndexPROGMEM(DATA_BOTH, command) != -1) {
            messToCamera[4] = data1;
            messToCamera[5] = data2;

        } else if (utils.searchIndexPROGMEM(SETPOS, command) != -1) {
            uint16_t angle = (uint16_t)(data1 % 35999); // Centième de degrès! avec max = 35999 (359.99 deg)

            messToCamera[4] = (uint8_t)((angle >> 0x08) & 0x00FF); // Get MSB
            messToCamera[5] = (uint8_t)(angle & 0x00FF);           // Get LSB

        } else if ((utils.searchIndexPROGMEM(QUERY_CMND, command) != -1 && !disableACK)) {
            if (utils.log_messages_) {
                sprintf_P(utils.log_buffer, (const char *)F("Cam %i: You are doing an query into send command ??????????"),
                          address);
                Serial.print(utils.log_buffer);
            }
            return false;
        } else {
            messToCamera[4] = 0x00;
            messToCamera[5] = data1;
        }
    }

    /////////////////

    messToCamera[6] = (messToCamera[1] + messToCamera[2] + messToCamera[3] + messToCamera[4] + messToCamera[5]) %
                      0x100; // Checksum modulo 0x100

    if (utils.log_messages_) { // log the message
        sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Sending message: "), address);
        Serial.print(utils.log_buffer);

        for (int i = 0; i < 7; i++) {
            sprintf(utils.log_buffer, "%02X", messToCamera[i]);
            Serial.print(utils.log_buffer);
            Serial.print(" ");
        }
        Serial.println();
    }

    (*SerialCamBus).write(messToCamera, sizeof(messToCamera)); // Write to the camera

    ///////////////////////////////////

    if (!disableACK) { // Check the response of the camera only if it isn't a request or the camera does not support
                       // return
        int timeout = 10000; // 10 millissecond wait

        if (!autoModule_)
            digitalWrite(rePin_, LOW); // Set the module at RX mode

        while (!(*SerialCamBus).available()) { // Wait for the first bit
            if (timeout == 0) {                // If timeout is reached
                if (utils.log_messages_) {
                    sprintf_P(utils.log_buffer,
                              (const char *)F("Cam %i: ERROR Could not verify camera ACK: timeout reached (is camera "
                                              "well plugged in?)\n"),
                              address);
                    Serial.print(utils.log_buffer);
                }
                if (!autoModule_)
                    digitalWrite(rePin_, HIGH); // set back at TX mode
                return false;
            }
            timeout--;
            delayMicroseconds(10);
        }

        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Reading Acknoledge from camera\n"), address);
            Serial.print(utils.log_buffer);
        }

        (*SerialCamBus).readBytes(ACKmessFromCamera, 4); // General response is 4 byte

        if (!autoModule_)
            digitalWrite(rePin_, HIGH); // set back at TX mode

        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Received ACK data (may be wrong):   "), address);
            Serial.print(utils.log_buffer);

            for (int i = 0; i < 7; i++) {
                sprintf(utils.log_buffer, "%02X ", messFromcamera[i]);
                Serial.print(utils.log_buffer);
            }
            Serial.println();
        }

        if (ACKmessFromCamera[0] != 0xFF) { // Check sync byte and checksum of the previous comand
            if (utils.log_messages_) {
                sprintf_P(
                    utils.log_buffer,
                    (const char *)F(
                        "Cam %i: ERROR Could not verify camera ACK: bad sync byte (is camera well plugged in?)\n"),
                    address);
                Serial.print(utils.log_buffer);
            }
            return false;
        }

        if (ACKmessFromCamera[1] != address) { // Check adress byte
            if (utils.log_messages_) {
                sprintf_P(utils.log_buffer,
                          (const char *)F(
                              "Cam %i: ERROR Could not verify camera ACK: bad address (is camera well plugged in?)\n"),
                          address);
                Serial.print(utils.log_buffer);
            }
            return false;
        }

        if (ACKmessFromCamera[2] != 0x00) { // check the always 0 byte (alarm byte)
            if (utils.log_messages_) {
                sprintf_P(
                    utils.log_buffer,
                    (const char *)F(
                        "Cam %i: ERROR Could not verify camera ACK: bad null ???? (is camera well plugged in?)\n"),
                    address);
                Serial.print(utils.log_buffer);
            }
            return false;
        }

        if (ACKmessFromCamera[3] != messToCamera[6]) { // Check the checksum
            if (utils.log_messages_) {
                sprintf_P(utils.log_buffer,
                          (const char *)F(
                              "Cam %i: ERROR Could not verify camera ACK: bad checksum (is camera well plugged in?)\n"),
                          address);
                Serial.print(utils.log_buffer);
            }
            return false;
        }

        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Message sent and ACK received!\n"), address);
            Serial.print(utils.log_buffer);
        }
        return true;
    }
}

/*!
 *  @brief  Send a query to the camera and reads the response
 *
 *  @param  address The address of the camera
 *  @param  request the wanted reponse to query
 *  @param  timeout default 1000; timeout for waiting a response of the camera
 *
 * @return true if succeded, false if an error occured
 */

uint16_t PelcoBus::request(uint8_t address, uint8_t request, int timeout) {
    byte response_command;

    if (utils.searchIndexPROGMEM(QUERY_CMND, request) != -1) {
        response_command = pgm_read_byte(&RESP_CMND[utils.searchIndexPROGMEM(QUERY_CMND, request)]); // Magic!
    } else {
        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: No valid request provided\n"), address);
            Serial.print(utils.log_buffer);
            Serial.println(request);
        }
        return -1;
    }

    command(address, true, request, 0x00, 0x00); // Send the query with ack diabled

    if (!autoModule_) {
        digitalWrite(rePin_, LOW); // Set the module at RX mode
    }

    byte buffer[7]; // Buffer for the reception from the camera

    while (!(*SerialCamBus).available()) { // Wait for the first bit
        if (timeout == 0) {                // If timeout is reached
            if (utils.log_messages_) {
                sprintf_P(utils.log_buffer, (const char *)F("Cam %i: ERROR timout reached\n"), address);
                Serial.print(utils.log_buffer);
            }
            if (!autoModule_) {
                digitalWrite(rePin_, HIGH); // set back at TX mode
            }
            return -1;
        }
        timeout--;
        delayMicroseconds(10);
    }

    //////Old solution
    /*      while(SerialCamBus.available()){//Do it until there is no more things to
       read buffer[index]=SerialCamBus.read(); Serial.printf("%02X ",buffer[index]);
            index++;
         }  */

    (*SerialCamBus).readBytes(buffer, 7); // Apparently this works

    if (!autoModule_) {
        digitalWrite(rePin_, HIGH); // set back at TX mode
    }

    int command_index = utils.searchIndex(buffer, response_command, 7); // Looks up where is the index of the response command

    if (command_index == -1) { // Checks if found
        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Warn: no reponse from camera (bad index)\n"), address);
            Serial.print(utils.log_buffer);
        }
        return -1;
    }

    // Checksum is sum of everything except sync modulo 0x100
    bool checksum = ((buffer[command_index] + buffer[command_index + 1] + buffer[command_index + 2]) % 0x100 ==
                     buffer[command_index + 3]);

    // Checking the sync byte, the null byte and the checksum
    if (buffer[command_index - 3] != 0xFF && buffer[command_index - 2 != 00] && !checksum) {
        return -1;
    }

    for (int i = 0; i < 7; i++) {
        messFromcamera[i] = buffer[(command_index - 3) + i];
    }

    if (utils.log_messages_) {
        sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Message from camera: "), address);
        Serial.print(utils.log_buffer);
        for (int i = 0; i < 7; i++) {
            sprintf(utils.log_buffer, "%02X ", messFromcamera[i]);
            Serial.print(utils.log_buffer);
        }
        Serial.println();
    }

    return (uint16_t)((((uint16_t)messFromcamera[4]) << 8) | ((uint16_t)messFromcamera[5])); // Return LSB data
}


/*!
 *  @brief  Send a raw command to the bus
 *
 *  @param  hex_string an hex String whose include all the command to make a camera move 
 *                      e.g: "FF 01 02 3F 00 00" checksum doesn't matter it will be reculated if there is an error
 *
 * @return true if succeded, false if an error occured
 */

// TODO: get the response if it is a query or extended one!!
bool PelcoBus::send_raw(String hex_string) {

    hex_string.replace(" ", ""); // Replace spaces

    int size = hex_string.length() / 2; // Size of the string without space
    byte raw_command[size];             // the final command
    char buffer[3];                     // A buffer, to convert the string into an array of char

    for (int i = 0; i <= size; i++) { // Loops to every char in in the string
        hex_string.substring(i * 2, i * 2 + 2)
            .toCharArray(buffer, sizeof(buffer));        // Slices the string into 2 pair of
                                                         // "bytes" and convert them into char arry
        raw_command[i] = (byte)strtol(buffer, NULL, 16); // conversion into byte
    }

    uint8_t address = raw_command[1]; // Address of the camera

    if (raw_command[0] != 0xFF) {
        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Wrong sync byte, updating to right sync byte\n"), address);
            Serial.print(utils.log_buffer);
        }
        raw_command[0] = 0xFF;
    }

    /////Check checksum if wrong, replace it
    uint8_t checksum = (raw_command[1] + raw_command[2] + raw_command[3] + raw_command[4] + raw_command[5]) % 0x100;

    if (checksum != raw_command[6]) {
        if (utils.log_messages_) {
            sprintf_P(utils.log_buffer, (const char *)F("Cam %i: Wrong checksum, updating to right checksum\n"), address);
            Serial.print(utils.log_buffer);
        }

        raw_command[6] = checksum;
    }

    if (utils.log_messages_) {
        sprintf_P(utils.log_buffer, (const char *)F("Cam %i: sending message "), address);
        Serial.print(utils.log_buffer);
        for (int i = 0; i < 7; i++) {
            sprintf(utils.log_buffer, "%02X ", messFromcamera[i]);
            Serial.print(utils.log_buffer);
        }
        Serial.println();
    }
    // write the command to the camera
    (*SerialCamBus).write(raw_command, sizeof(raw_command));

    return true;
}

