#ifndef constants_H
#define constants_H

//////////////////////////////////////////Some constants stored in Progmem

//////////////////////////////////////CMND1
/////Basic commands bit 5 &6 are always 0
#define FOCUS_N 0x61 //0x01 + 0b0110 0000

#define IRIS_O 0x62 //0x02 + 0b0110 0000
#define IRIS_C 0x64 //0x04 + 0b0110 0000

#define OFF 0x68 //0x08 + 0b0110 0000 //What does it do?
#define ON 0xE8  //0x88 + 0b0110 0000 //What does it do?

//////////////////////////////////////CMND2
/////Basic commands
#define STOP 0x00

#define PAN_R 0x02
#define PAN_L 0x04

#define TILT_U 0x08
#define TILT_D 0x10

#define ZOOM_T 0x20
#define ZOOM_W 0x40

#define FOCUS_F 0x80

#define PAN_L_TILT_U 0x0C
#define PAN_R_TILT_U 0x0A
#define PAN_R_TILT_D 0x12
#define PAN_L_TILT_D 0x14

/////Complex commands
#define SET_PRESET 0x03  // byte6: preset id
#define CLR_PRESET 0x05  // byte6: preset id
#define GOTO_PRESET 0x07 // byte6: preset id

// Predifined preset (everything is call (goto) unless mentioned otherwise):
#define PRESET_FLIP 0x21
#define PRESET_ZERO_PAN 0x22
#define PRESET_AUX_1 0x54 // set: on ; call:off
#define PRESET_AUX_2 0x55 // set: on ; call:off
#define PRESET_WIPER 0x56
#define PRESET_WASHER 0x57
#define PRESET_IR_IN 0x58
#define PRESET_IR_OUT 0x59
//#define PRESET_CLEAR 0x5E  NOT CLEAR????
#define PRESET_MAIN_MENU 0x5F // Brings up the main menu (set)
//

#define SET_AUX 0x09   // byte6: aux id
#define CLEAR_AUX 0x0B // byte6: aux id

#define DUMMY 0x0D //??

#define REMOTE_RESET 0x0F

#define WRITE_CHAR 0x15 // Byte5: column Byte6: ASCII code
#define CLEAR_SCR 0x17

#define ALARM_ACK 0x19 // byte6: ALARM id; ????

#define SET_ZOOM_SPEED 0x25  // Param from 00 to 03
#define SET_FOCUS_SPEED 0x27 // Param from 00 to 03

#define RESET 0x29

#define AUTO_FOCUS 0x2B             // Param from 00 to 02 (auto/on/off)
#define AUTO_IRIS 0x2D              // Param from 00 to 02 (auto/on/off)
#define AUTO_AGC 0x2F               // Param from 00 to 02 (auto/on/off)
#define BACKLIGHT_COMPENSATION 0x31 // Param from 00 to 01 (on/off)
#define AUTO_WHITE_BALANCE 0x33     // Param from 00 to 01 (on/off)

#define QUERY 0x45 //???????????

#define ZERO_POS 0x49
#define SET_PAN 0x4B  // b5: MSB and b6: LSB //max 8C 59 todo: check msb and lsb
#define SET_TILT 0x4D // b5: MSB and b6: LSB
#define SET_ZOOM 0x4F // b5: MSB and b6: LSB MAX:  (desired zoom position / zoom limit) * 65535

#define QUERY_PAN 0x51
#define QUERY_TILT 0x53
#define QUERY_ZOOM 0x55

#define RESP_PAN 0x59  // b5: MSB and b6: LSB
#define RESP_TILT 0x5B // b5: MSB and b6: LSB
#define RESP_ZOOM 0x5D // b5: MSB and b6: LSB

/*

A lot of commands is not implemented yet, it is not "important" commands for now message me via github if you wanna get
a command
 */
//Special commands:

const byte CMND1[5] PROGMEM = {FOCUS_N, IRIS_O, IRIS_C, OFF, ON}; //List of all commands that go on the byte3; a list with command 2 is not required, a bit of logic please
const byte DATA1[2] PROGMEM = {PAN_L, PAN_R}; //List of all commands that use ONLY the DATA1 for data pass

const byte SETPOS[3] PROGMEM = {SET_PAN, SET_TILT, SET_ZOOM}; //List of all command that require an MSB and LSB

const byte DATA_BOTH[5] PROGMEM= {PAN_L_TILT_D, PAN_L_TILT_U, PAN_R_TILT_D, PAN_R_TILT_U, WRITE_CHAR};//List of all commands that use the DATA1 and DATA2 for data pass (excluding the SETPOS' commands)

const byte RESP_CMND[3] PROGMEM = {RESP_PAN, RESP_TILT, RESP_ZOOM}; 

const byte QUERY_CMND[3] PROGMEM = {QUERY_PAN, QUERY_TILT, QUERY_ZOOM}; 

const byte PRESET_CMND[3] PROGMEM= {SET_PRESET, CLR_PRESET, GOTO_PRESET}; //will I use it?


///////////////////////////////////////////////// Constants for the config parameter

// D-protocol baud self-explaintory
#define PELCO_D2400 0xD24
#define PELCO_D4800 0xD48
#define PELCO_D9600 0xD96


//////////////////////////////////////////::

#define NOT_A_PIN -1

#endif