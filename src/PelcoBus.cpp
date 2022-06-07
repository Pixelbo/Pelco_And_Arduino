/*!
 *  @file PelcoBus.cpp
 *
 *  @mainpage Manages all the bus communication with the Pelco cameras.
 *
 *  @section  Written by Boris Hilkens 05/06/2022
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */



#include <SoftwareSerial.h>

#include "Arduino.h"
#include "constants.h"
#include "PelcoBus.h"

/*!
 *  @brief  Constructor
 *
 *  @param  baud baud of the cameras
 *  @param  rxPin the RX pin of the module (Cam to Arduino)
 *  @param  txPin the TX pin of the module (Arduino to Cam)
 *  @param  log_messages Print log messages of this camera
 *  @param  readEnPin Pin for RS485 modules that require toggle between rx and
 * tx   (RE and \DE pin of the module)
 *
 */

PelcoBus::PelcoBus() {
    return;
}

PelcoBus::PelcoBus(uint8_t txPin, uint8_t rxPin, uint8_t readEnPin) {
    txPin_ = txPin;
    rxPin_ = rxPin;
    rePin_ = readEnPin;
}

/*!
 *  @brief  begin everything and check some things
 *
 */

void PelcoBus::begin(uint32_t config, bool log_messages = false) {
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

    if (log_messages) {
/*         if (!Serial) {
            Serial.begin(9600);
        } */
        Serial.print(F("Message log has been activated for the Pelco Bus!\n"));

        sprintf_P(log_buffer, (const char * ) F("PelcoBus config: baud=%u and protocol D \n"), baud);
        Serial.print(log_buffer);

        log_messages_ = true;
    } else {
        log_messages_ = false;
    }

    if (rePin_ == NOT_A_PIN) {
        autoModule_ = true; // Is the module an auto switching between tx and RX ?
    } else {
        pinMode(rePin_, OUTPUT);
        autoModule_ = false;
    }

    pinMode(txPin_, OUTPUT);
    pinMode(rxPin_, INPUT);

    if (!autoModule_)
        digitalWrite(rePin_, HIGH); // Set the module in tx mode

    SerialCamBus = new SoftwareSerial(rxPin_, txPin_);
    (*SerialCamBus).begin(baud);
}

/*!
 *  @brief  Send message to the camera
 *
 *  @param  address Address of the camera
 *  @param  command the wanted command (see header)
 *  @param  data1 Main parameter
 *  @param  data2 Second parameter for command that requires 2 parameters
 *  @return true if succeed, false if not succeed
 *
 */

bool PelcoBus::command(uint8_t address, bool disableACK , uint8_t command, uint16_t data1 = 0x00, uint8_t data2 = 0x00) {
    messToCamera[0] = 0xFF;    // The first byte is always FF (sync)
    messToCamera[1] = address; // the second is the adress

    // Special commands handling:
    if (searchIndexPROGMEM(CMND1, command) != -1) {
        messToCamera[2] = command - 0b01100000; // Minus the thing that differentiate from others
        messToCamera[3] = 0x00;
        messToCamera[4] = 0x00;
        messToCamera[5] = 0x00;

    } else { // that means command is the byte 4
        messToCamera[2] = 0x00;
        messToCamera[3] = command;

        if (searchIndexPROGMEM(DATA1, command) != -1) {
            messToCamera[4] = data1;
            messToCamera[5] = 0x00;

        } else if (searchIndexPROGMEM(DATA_BOTH, command) != -1) {
            messToCamera[4] = data1;
            messToCamera[5] = data2;

        } else if (searchIndexPROGMEM(SETPOS, command) != -1) {
            uint16_t angle = (uint16_t)(data1 % 35999); // Centième de degrès! avec max = 35999 (359.99 deg)

            messToCamera[4] = (uint8_t)((angle >> 0x08) & 0x00FF); // Get MSB
            messToCamera[5] = (uint8_t)(angle & 0x00FF);           // Get LSB

        } else if ((searchIndexPROGMEM(QUERY_CMND, command) != -1 && !disableACK)) {
            if (log_messages_) {
                sprintf_P(log_buffer, (const char * ) F("Cam %i: You are doing an query into send command ??????????"), address);
                Serial.print(log_buffer);
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

    if (log_messages_) { // log the message
        sprintf_P(log_buffer, (const char * ) F("Cam %i: Sending message: "), address);
        Serial.print(log_buffer);
        for (int i = 0; i < 7; i++) {
            sprintf(log_buffer, "%02X", messToCamera[i]);
            Serial.print(log_buffer);
            Serial.print(" ");
        }
        Serial.println();
    }

    (*SerialCamBus).write(messToCamera, sizeof(messToCamera)); // Write to the camera

    if ((!disableACK) && (!disableACK)) { // Check the response of the camera only if it isn't a request or the camera does not support return
        int timeout = 10000; // 10 millissecond wait

        if (!autoModule_)
            digitalWrite(rePin_, LOW); // Set the module at RX mode

        while (!(*SerialCamBus).available()) { // Wait for the first bit
            if (timeout == 0) {                // If timeout is reached
                if (log_messages_) {
                    sprintf_P(log_buffer,
                     (const char * ) F("Cam %i: ERROR Could not verify camera ACK: timeout reached (is camera well plugged in?)\n"),
                            address);
                    Serial.print(log_buffer);
                }
                if (!autoModule_)
                    digitalWrite(rePin_, HIGH); // set back at TX mode
                return false;
            }
            timeout--;
            delayMicroseconds(10);
        }
        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: Reading Acknoledge from camera\n"), address);
            Serial.print(log_buffer);
        }

        (*SerialCamBus).readBytes(ACKmessFromCamera, 4); // General response is 4 byte

        if (!autoModule_)
            digitalWrite(rePin_, HIGH); // set back at TX mode

        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: Received ACK data (may be wrong):   "), address);
            Serial.print(log_buffer);

            for (int i = 0; i < 7; i++) {
                sprintf(log_buffer, "%02X ", messFromcamera[i]);
                Serial.print(log_buffer);
            }
            Serial.println();
        }

        if (ACKmessFromCamera[0] != 0xFF) { // Check sync byte and checksum of the previous comand
            if (log_messages_) {
                sprintf_P(log_buffer,
                    (const char * ) F("Cam %i: ERROR Could not verify camera ACK: bad sync byte (is camera well plugged in?)\n"),
                        address);
                Serial.print(log_buffer);
            }
            return false;
        }

        if (ACKmessFromCamera[1] != address) { // Check adress byte
            if (log_messages_) {
                sprintf_P(log_buffer,
                    (const char * ) F("Cam %i: ERROR Could not verify camera ACK: bad address (is camera well plugged in?)\n"),
                        address);
                Serial.print(log_buffer);
            }
            return false;
        }

        if (ACKmessFromCamera[2] != 0x00) { // check the always 0 byte (alarm byte)
            if (log_messages_) {
                sprintf_P(log_buffer,
                    (const char * ) F("Cam %i: ERROR Could not verify camera ACK: bad null ???? (is camera well plugged in?)\n"),
                        address);
                Serial.print(log_buffer);
            }
            return false;
        }

        if (ACKmessFromCamera[3] != messToCamera[6]) { // Check the checksum
            if (log_messages_) {
                sprintf_P(log_buffer,
                    (const char * ) F("Cam %i: ERROR Could not verify camera ACK: bad checksum (is camera well plugged in?)\n"),
                        address);
                Serial.print(log_buffer);
            }
            return false;
        }

        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: Message sent and ACK received!\n"), address);
            Serial.print(log_buffer);
        }
        return true;
    }
}

/*!
 *  @brief  Send a query to the camera and reads the response
 *
 *  @param  request the wanted reponse (see header)
 *  @param  timeout default 1000; timeout for waiting a byte
 *  @param  maxbuffer Maximum size of the buffer; defaut 20
 *
 * @return true if succeded, false if an error occured
 */

uint16_t PelcoBus::request(uint8_t address, uint8_t request, int timeout = 1000) {
    byte response_command;

    if (searchIndexPROGMEM(QUERY_CMND, request) != -1) {
        response_command = pgm_read_byte(&RESP_CMND[searchIndexPROGMEM(QUERY_CMND, request)]); // Magic!
    } else {
        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: No valid request provided\n"), address);
            Serial.print(log_buffer);
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
            if (log_messages_) {
                sprintf_P(log_buffer, (const char * ) F("Cam %i: ERROR timout reached\n"), address);
                Serial.print(log_buffer);
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

    int command_index = searchIndex(buffer, response_command, 7); // Looks up where is the index of the response command

    if (command_index == -1) { // Checks if found
        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: Warn: no reponse from camera (bad index)\n"), address);
            Serial.print(log_buffer);
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

    if (log_messages_) {
        sprintf_P(log_buffer, (const char * ) F("Cam %i: Message from camera: "), address);
        Serial.print(log_buffer);
        for (int i = 0; i < 7; i++) {
            sprintf(log_buffer, "%02X ", messFromcamera[i]);
            Serial.print(log_buffer);
        }
        Serial.println();
    }

    return (uint16_t)((((uint16_t)messFromcamera[4]) << 8) | ((uint16_t)messFromcamera[5])); // Return LSB data
}

////////todo get the response if it is a query or extended one!!
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
        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: Wrong sync byte, updating to right sync byte\n"), address);
            Serial.print(log_buffer);
        }
        raw_command[0] = 0xFF;
    }

    /////Check checksum
    uint8_t checksum = (raw_command[1] + raw_command[2] + raw_command[3] + raw_command[4] + raw_command[5]) % 0x100;
    if (checksum != raw_command[6]) {
        if (log_messages_) {
            sprintf_P(log_buffer, (const char * ) F("Cam %i: Wrong checksum, updating to right checksum\n"), address);
            Serial.print(log_buffer);
        }

        raw_command[6] = checksum;
    }

    if (log_messages_) {
        sprintf_P(log_buffer, (const char * ) F("Cam %i: sending message "), address);
        Serial.print(log_buffer);
        for (int i = 0; i < 7; i++) {
            sprintf(log_buffer, "%02X ", messFromcamera[i]);
            Serial.print(log_buffer);
        }
        Serial.println();
    }
    // write the command to the camera
    (*SerialCamBus).write(raw_command, sizeof(raw_command));

    return true;
}


/*!
 *  @brief  Search a value trough a array of bytes one function is for progmem the other one isn't for progmem
 *
 *  @param  look_array the array
 *  @param  value value to lookup
 *
 * @return the index of the element found
 */

int PelcoBus::searchIndexPROGMEM(const byte look_array[], byte value) {
    int i = 0;
    for (i = 0; i <= (sizeof(look_array) / sizeof(*look_array)); i++) {
        if (pgm_read_byte(&look_array[i]) == value) {
            return i;
        }
    }
    return -1;
}

int PelcoBus::searchIndex(byte look_array[], byte value,
                             size_t size) { // For an x or y reason sizeof don't work properly

    int i = 0;
    for (i = 0; i <= (size); i++) {

        if (look_array[i] == value) {
            return i;
        }
    }
    return -1;
}