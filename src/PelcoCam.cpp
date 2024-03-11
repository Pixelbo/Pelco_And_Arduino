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
#include "utils.h"

#include "constants.h"

/*!
 *  @brief  Get an new object: the PelcoCam used to controls camera without bothering with address, etc
 *
 *  @param  bus The pointer to the PelcoBus object
 *  @param  address The camera's address
 *  @param  disable_ack Disable reading response and acknowledge from camera
 */

PelcoCam::PelcoCam(PelcoBus *bus, uint8_t address, bool disable_ack) {
    bus_ = bus; // Get the address (pointer) of the PelcoBus class
    address_ = address;
    disable_ack_ = disable_ack;
}

/*!
 *  @brief  See if the camera is avaible/online ONLY WORKS WITH disable_ack at false
 *
 *  @return true if the camera is online false if the program didn't get a response from the camera
 */

bool PelcoCam::available() { // Returns true if there's a ACK
    if (disable_ack_) {
        if (utils.log_messages_) {
            sprintf_P(
                utils.log_buffer,
                (const char *)F("Cam %i: ERROR You can not get a Acknowledge when your camera doesn't support it! \n"),
                address_);
            Serial.print(utils.log_buffer);
        }
        return true;
    }
    return bus_->command(address_, disable_ack_, DUMMY);
}

/*!
 *  @brief  Simple shortcuts to commands
 *
 *  @return true if everything is ok
 */

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

/*!
 *  @brief  Sends a command to the pelcobus
 *
 *  @param command The command to pass to the bus
 *  @param data1 data1
 *  @param data2 data2
 * 
 *  @return true if everything is ok
 */

bool PelcoCam::send_command(uint8_t command, uint16_t data1, uint8_t data2) {
    return bus_->command(address_, disable_ack_, command, data1, data2);
}

/*!
 *  @brief  Sends a request to the pelcobus and get a respinse
 *
 *  @param request the request to pass to the bus
 *  @param timeout Timeout time for getting a response
 * 
 *  @return The reponse of the camera
 */

uint16_t PelcoCam::send_request(uint8_t request, uint16_t timeout) {
    if (disable_ack_) {
        if (utils.log_messages_) {
            sprintf_P(
                utils.log_buffer,
                (const char *)F("Cam %i: ERROR You can not get a Response when your camera doesn't support it! \n"),
                address_);
            Serial.print(utils.log_buffer);
        }
        return 0;
    }

    return bus_->request(address_, request, timeout);
}