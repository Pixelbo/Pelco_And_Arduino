/*!
 *  @file PelcoCam.h
 *
 *  @mainpage Manages a single camera
 *
 *  @section  Written by Boris Hilkens 05/06/2022
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include <SoftwareSerial.h>

#include "Arduino.h"

#include "utils.h"
#include "PelcoBus.h"

class PelcoCam {
  private:
    uint8_t address_;
    bool disable_ack_;
    PelcoBus *bus_;

    Utils utils;

  public:
    PelcoCam(PelcoBus *bus, uint8_t address, bool disable_ack = false);

    bool available();

    bool menu_open();
    bool menu_up();
    bool menu_down();
    bool menu_enter();
    bool menu_back();

    bool send_command(uint8_t command = STOP, uint16_t data1 = 0x00, uint8_t data2 = 0x00);
    uint16_t send_request(uint8_t request, uint16_t timeout = 1000);
};