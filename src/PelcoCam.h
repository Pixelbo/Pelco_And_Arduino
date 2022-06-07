/*!
 *  @file PelcoCam.h
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

#include "PelcoBus.h"

class PelcoCam: public PelcoBus{
    private:
        uint8_t address_;
        bool disable_ack_;

    public:
        PelcoCam(uint8_t address, bool disable_ack = false);

        bool available();

        bool send_command(uint8_t command, uint16_t data1 = 0x00, uint8_t data2 = 0x00);
        uint16_t send_request(uint8_t request, uint16_t timeout = 1000);
};