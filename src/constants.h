#ifndef constants_H
#define constants_H

//////////////////////////////////////////Some constants stored in Progmem
const uint8_t PROGMEM PAN_L = 0x04;
const uint8_t PROGMEM PAN_R = 0x02;
const uint8_t PROGMEM TILT_U = 0x08;
const uint8_t PROGMEM TILT_D = 0x10;

const uint8_t PROGMEM PAN_L_TILT_U = 0x0C;
const uint8_t PROGMEM PAN_R_TILT_U = 0x0A;
const uint8_t PROGMEM PAN_L_TILT_D = 0x14;
const uint8_t PROGMEM PAN_R_TILT_D = 0x12;

const uint8_t PROGMEM ZOOM_T = 0x20;
const uint8_t PROGMEM ZOOM_W = 0x40;
const uint8_t PROGMEM SET_ZOOM_SPEED = 0x25; // Param from 00 to 03

const uint8_t PROGMEM FOCUS_F = 0x80;
const uint8_t PROGMEM FOCUS_N = 0x01;         //!!!!not byte 4 but 3
const uint8_t PROGMEM SET_FOCUS_SPEED = 0x27; // Param from 00 to 03
const uint8_t PROGMEM AUTO_FOCUS = 0x2B;      // Param from 00 to 02 (auto/on/off)

const uint8_t PROGMEM STOP = 0x00;

const uint8_t PROGMEM ON = 0x88; //!!!!not byte 4 but 3
const uint8_t PROGMEM OFF = 0x08;

const uint8_t PROGMEM RESET = 0x29;

const uint8_t PROGMEM SET_PRESET = 0x03;  // data preset id
const uint8_t PROGMEM GOTO_PRESET = 0x05; // data preset id
const uint8_t PROGMEM CLR_PRESET = 0x07;  // data preset id

const uint8_t PROGMEM QUERY_PAN = 0x51;
const uint8_t PROGMEM QUERY_TILT = 0x53;
const uint8_t PROGMEM QUERY_ZOOM = 0x55;
const uint8_t PROGMEM QUERY_FOCUS = 0x61;

const uint8_t PROGMEM RESP_PAN = 0x59;
const uint8_t PROGMEM RESP_TILT = 0x5B;
const uint8_t PROGMEM RESP_ZOOM = 0x5D;
const uint8_t PROGMEM RESP_FOCUS = 0x63;

/*

A lot of commands is not implemented yet, I'll do it in the future!
 */

///////////////////////////////////////////////// Constants for the config parameter

#define D true
#define P false

// D-protocol baud self-explaintory
#define PELCO_D2400 0xD24
#define PELCO_D4800 0xD48
#define PELCO_D9600 0xD96
// P-protocol baud self-explaintory
#define PELCO_P2400 0xE24
#define PELCO_P4800 0xE48
#define PELCO_P9600 0xE96

//////////////////////////////////////////::

#define NOT_A_PIN -1

#endif