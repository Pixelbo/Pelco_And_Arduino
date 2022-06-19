/*!
 *  @file PelcoCam.cpp
 *
 *  @mainpage Manages a single camera
 *
 *  @section  Written by Boris Hilkens 05/06/2022
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include <SoftwareSerial.h>

#include "Arduino.h"

#include "PelcoCam.h"
#include "constants.h"

// Only serves as a wrapper for the PelcoBus class

PelcoCam::PelcoCam(PelcoBus *bus, uint8_t address, bool disable_ack) {
    bus_ = bus; // Get the address (pointer) of the PelcoBus class
    address_ = address;
    disable_ack_ = disable_ack;
}

bool PelcoCam::available() { // Returns true if there's a ACK
    if (disable_ack_) {
        if (log_messages_) {
            sprintf_P(
                log_buffer,
                (const char *)F("Cam %i: ERROR You can not get a Acknowledge when your camera doesn't support it! \n"),
                address_);
            Serial.print(log_buffer);
        }
        return false;
    }
    return bus_->command(address_, disable_ack_, DUMMY);
}

bool PelcoCam::menu_open() {
    return bus_->command(address_, disable_ack_, SET_PRESET, 95);
}
bool PelcoCam::menu_up() {
    return bus_->command(address_, disable_ack_, TILT_D, 0x10);
}
bool PelcoCam::menu_down() {
    return bus_->command(address_, disable_ack_, TILT_U, 0x10);
}
bool PelcoCam::menu_enter() {
    return bus_->command(address_, disable_ack_, IRIS_O);
}
bool PelcoCam::menu_back() {
    return bus_->command(address_, disable_ack_, IRIS_C);
}

bool PelcoCam::send_command(uint8_t command, uint16_t data1, uint8_t data2) {
    return bus_->command(address_, disable_ack_, command, data1, data2);
}

uint16_t PelcoCam::send_request(uint8_t request, uint16_t timeout) {
    if (disable_ack_) {
        if (log_messages_) {
            sprintf_P(
                log_buffer,
                (const char *)F("Cam %i: ERROR You can not get a Response when your camera doesn't support it! \n"),
                address_);
            Serial.print(log_buffer);
        }
        return 0;
    }

    return bus_->request(address_, request, timeout);
}