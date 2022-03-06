/*!
 *  @file Pelco_And_Arduino.cpp
 *
 *  @mainpage The root of this libray!
 *
 *  @section author Written by Boris Hilkens.
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include "Pelco_And_Arduino.h"

#include <SoftwareSerial.h>

#include "Arduino.h"
#include "constants.h"

/*!
 *  @brief  Constructor
 *
 *  @param  Address The address of the camera
 *  @param  baud baud of the cameras
 *  @param  txPin the TX pin of the module (Arduino to Cam)
 *  @param  rxPin the RX pin of the module (Cam to Arduino)
 *  @param  log_messages Print log messages of this camera
 *  @param  readEnPin Pin for RS485 modules that require toggle between rx and
 * tx   (RE and \DE pin of the module)
 *
 */

PelcoCam::PelcoCam(uint8_t address, uint32_t config, uint8_t txPin, uint8_t rxPin, bool log_messages,
                   uint8_t readEnPin) {
    address_ = address;
    config_ = config;
    txPin_ = txPin;
    rxPin_ = rxPin;
    rePin_ = readEnPin;
    log_messages_ = log_messages;
}

/*!
 *  @brief  begin everything and check some things
 *
 */

void PelcoCam::begin() {
    if (log_messages_) {
        Serial.begin(115200);
        Serial.printf("Message log has been activated for the camera %02X !\n", address_);
    }

    uint16_t baud;

    switch (config_) {
    case PELCO_D2400:
        protocol = D;
        baud = 2400;
        break;
    case PELCO_D4800:
        protocol = D;
        baud = 4800;
        break;
    case PELCO_D9600:
        protocol = D;
        baud = 9600;
        break;

    case PELCO_P2400:
        protocol = P;
        baud = 2400;
        break;
    case PELCO_P4800:
        protocol = P;
        baud = 4800;
        break;
    case PELCO_P9600:
        protocol = P;
        baud = 9600;
        break;
    }

    if (log_messages_)
        Serial.printf("Cam %i: Pelco config: baud=%u and protocol=%d (0 for D and 1 for P) \n", address_, baud, protocol);

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

    SerialCam.begin(baud, SWSERIAL_8N1, rxPin_, txPin_);
}

/*!
 *  @brief  Send message to the camera
 *
 *  @param  command the wanted command (see header)
 *  @param  params Main parameter
 *  @param  params2 Second parameter for command that requires 2 parameters
 *  @param  params2 Second parameter for command that requires 2 parameters
 *
 *  @return true if succeed false if not succeed
 *
 */

bool PelcoCam::send_command(uint8_t command, uint8_t params, uint8_t params2, bool request) {
    messToCamera[0] = 0xFF;
    messToCamera[1] = address_;

    if (command == ON || command == OFF || command == FOCUS_N) { // If the command is on or off, set the command to
                                                                 // uint8_t 3
        messToCamera[2] = command;
        messToCamera[3] = 0x00;
    } else {
        messToCamera[2] = 0x00;
        messToCamera[3] = command;
    }
    if (command == PAN_L || command == PAN_R) {
        messToCamera[4] = params;
        messToCamera[5] = 0x00;
    } else if (command == PAN_L_TILT_U || command == PAN_R_TILT_U || command == PAN_L_TILT_D ||
               command == PAN_R_TILT_D) {
        messToCamera[4] = params2;
        messToCamera[5] = params;
    } else {
        messToCamera[4] = 0x00;
        messToCamera[5] = params;
    }

    messToCamera[6] =
        (messToCamera[1] + messToCamera[2] + messToCamera[3] + messToCamera[4] + messToCamera[5]) % 0x100; // Checksum

    if (log_messages_) {
        Serial.printf("Cam %i: Sending message: ", address_);
        for (int i = 0; i < 7; i++) {
            Serial.printf("%02X", messToCamera[i]);
            Serial.print(" ");
        }
        Serial.println();
    }

    SerialCam.write(messToCamera, sizeof(messToCamera));

    if (!request) {          // Check the response of the camera
        int timeout = 10000; // 10 millissecond wait
        byte buffer[4];      // Buffer for the reception from the camera

        if (!autoModule_)
            digitalWrite(rePin_, LOW); // Set the module at RX mode

        while (!SerialCam.available()) { // Wait for the first bit
            if (timeout == 0) {          // If timeout is reached
                if (log_messages_)
                    Serial.printf("Cam %i: ERROR Could not verify camera reponse: timeout reached (is camera well plugged in?)\n", address_);
                if (!autoModule_)
                    digitalWrite(rePin_, HIGH); // set back at TX mode
                return false;
            }
            timeout--;
            delayMicroseconds(10);
        }

        SerialCam.readBytes(buffer, 4);

        if (!autoModule_)
            digitalWrite(rePin_, HIGH); // set back at TX mode

        int command_index = searchIndex(buffer, messToCamera[6]); // Looks up where is the index of the response TODO: verify every command

        if (command_index != 3) { // Checks if found
            if(messToCamera[6] != 0x01){//Filter stop command cause checksum==1
                if (log_messages_)
                    Serial.printf("Cam %i: ERROR Could not verify camera reponse: bad index (is camera well plugged in?)\n", address_);
                return false;
            }
        }

        if (buffer[command_index - 3] != 0xFF) { // Check sync byte and checksum of the previous comand
        if(messToCamera[6] != 0x01){//Filter stop command cause checksum==1
            if (log_messages_)
                Serial.printf("Cam %i: ERROR Could not verify camera reponse: bad sync byte (is camera well plugged in?)\n", address_);
            return false;
        }
        }

        if (log_messages_)
            Serial.printf("Cam %i: Message sent and ACK received!\n", address_);
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

int PelcoCam::send_request(uint8_t request, uint timeout, uint maxbuffer) {
    byte response_command;

    if (request == QUERY_PAN) {
        response_command == RESP_PAN;
    } else if (request == QUERY_TILT) {
        response_command = RESP_TILT;
    } else if (request == QUERY_ZOOM) {
        response_command = RESP_ZOOM;
    } else if (request == QUERY_FOCUS) {
        response_command = RESP_FOCUS;
    } else {
        if (log_messages_)
            Serial.printf("Cam %i: No valid request provided\n", address_);
        return -1;
    }

    send_command(request, 0x00, 0x00, true); // Send the query

    if (!autoModule_)
        digitalWrite(rePin_, LOW); // Set the module at RX mode

    byte buffer[maxbuffer]; // Buffer for the reception from the camera

    while (!SerialCam.available()) { // Wait for the first bit
        if (timeout == 0) {          // If timeout is reached
            if (log_messages_)
                Serial.printf("Cam %i: ERROR timout reached\n", address_);
            if (!autoModule_)
                digitalWrite(rePin_, HIGH); // set back at TX mode
            return -1;
        }
        timeout--;
        delayMicroseconds(10);
    }

    //////Old solution
    /*      while(SerialCam.available()){//Do it until there is no more things to
       read buffer[index]=SerialCam.read(); Serial.printf("%02X ",buffer[index]);
            index++;
         }  */

    SerialCam.readBytes(buffer, 7); // Apparently this works

    if (!autoModule_)
        digitalWrite(rePin_, HIGH); // set back at TX mode

    int command_index = searchIndex(buffer, 0x59); // Looks up where is the index of the response command

    /* A theorical response:
    FF   00   59      4A  13  B7
    sync null command msb lsb checksum

    A practical response (what does the arduino reads):
        FF 00 FF 00 59 4A 13 B7 FF 01 48
    */

    if (command_index == -1) { // Checks if found

        if (log_messages_)
            Serial.printf("Cam %i: Warn: no reponse from camera (bad index)\n", address_);
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
        Serial.printf("Cam %i: Message from camera: ", address_);
        for (int i = 0; i < 7; i++) {
            Serial.printf("%02X ", messFromcamera[i]);
        }
        Serial.println();
    }

    return messFromcamera[5]; // Return LSB data
}

void PelcoCam::send_raw(String hex_string) {
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

    if (raw_command[0] != 0xFF)
        return; // Check sync byte

    /////Check checksum
    uint8_t checksum = (raw_command[1] + raw_command[2] + raw_command[3] + raw_command[4] + raw_command[5]) % 0x100;
    if (checksum != raw_command[6]) {
        if (log_messages_)
            Serial.printf("Cam %i: Wrong chacksum, updating to right checksum\n", address_);

        raw_command[6] = checksum;
    }

    if (log_messages_) {
        Serial.printf("Cam %i: Sending message: ", address_);
        for (int i = 0; i < size; i++) {
            Serial.printf("%02X", raw_command[i]);
            Serial.print(" ");
        }
        Serial.println();
    }
    // write the command to the camera
    SerialCam.write(raw_command, sizeof(raw_command));
}

/*!
 *  @brief  Dearch a value trough a array of bytes
 *
 *  @param  look_array the array
 *  @param  value value to lookup
 *
 * @return the index of the element found
 */

int PelcoCam::searchIndex(byte look_array[], byte value) {
    for (int i = 0; i < sizeof(look_array) / sizeof(look_array[0]); i++) {
        if (look_array[i] == value) {
            return i;
        }
    }
    return -1;
}