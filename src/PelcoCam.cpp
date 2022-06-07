/*!
 *  @file PelcoCam.cpp
 *
 *  @mainpage Manages a single camera
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

#include "PelcoCam.h"

// Only serves as a wrapper for the PelcoBus class

PelcoCam::PelcoCam(uint8_t address, bool disable_ack): PelcoBus(PelcoBus::rxPin_, PelcoBus::txPin_, PelcoBus::rePin_){
    address_ = address;
    disable_ack_ = disable_ack;
}

bool PelcoCam::available() { // Returns true if there's a ACK
    if (disable_ack_) {
        if (log_messages_) {
            sprintf_P(log_buffer,
                    (const char * ) F("Cam %i: ERROR You can not get a Acknoledge when your camera doesn't support it! \n"),
                    address_);
            Serial.print(log_buffer);
        }
        return false;
    }
        return PelcoBus::command(address_, disable_ack_, DUMMY);
    
}

bool PelcoCam::send_command(uint8_t command, uint16_t data1 = 0x00, uint8_t data2 = 0x00) {
    Serial.print("ah");
    Serial.print(log_buffer);
    delay(1000);
    Serial.println("bbbb");
    bool test = PelcoBus::command(address_, disable_ack_, command, data1, data2);
    return test;
}

uint16_t PelcoCam::send_request(uint8_t request, uint16_t timeout = 1000) {
    if (disable_ack_) {
        if (log_messages_) {
            sprintf_P(log_buffer,
                    (const char * ) F("Cam %i: ERROR You can not get a Response when your camera doesn't support it! \n"),
                    address_);
            Serial.print(log_buffer);
        }
        return 0;
    }

    return PelcoBus::request(address_, request, timeout);
}