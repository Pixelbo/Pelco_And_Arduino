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

// Function comment

#include "Arduino.h"
#include <SoftwareSerial.h>

#include "Pelco_And_Arduino.h"

/*!
 *  @brief  Constructor
 * 
 *  @param  Address The address of the camera
 *  @param  baud baud of the camera
 * 
 *	@return returnValue description
 */

PelcoCam::PelcoCam(uint8_t Address, int baud, int txPin, int rxPin, bool log_messages=false){
    Address_ = Address;
    baud_ = baud;
    txPin_ = txPin;
    rxPin_ = rxPin;
    log_messages_ = log_messages;
    
    if(log_messages_){
        Serial.begin(115200);
        Serial.println("Message log has been activated!");
    }

    SoftwareSerial SerialCam(rxPin, txPin) ;
    SerialCam.begin(baud);
}

void PelcoCam::send_message(uint8_t command, uint8_t params=0x00, uint8_t params2=0x00){
  messToCamera[0] = 0xFF;
  messToCamera[1] = Address_;

  if(command == ON || command == OFF){ //If the command is on or off, set the command to byte 3
      messToCamera[2] = command;
      messToCamera[3] = 0x00;
  }else{
      messToCamera[2] = 0x00;
      messToCamera[3] = command;
  }

  if(command == PAN_L || command == PAN_R){
      messToCamera[4] = params;
  }else if(command == PAN_L_TILT_U || command == PAN_R_TILT_U || command == PAN_L_TILT_D || command == PAN_R_TILT_D){
      messToCamera[4] = params2;
      messToCamera[5] = params;
  }else{
      messToCamera[5] = params;
  }

  messToCamera[6] = (messToCamera[1] + messToCamera[2] + messToCamera[3] +messToCamera[4] +messToCamera[5])%0x100; //Checksum

  if(log_messages_){
    Serial.print("Sending message: ");
    logMessage(messToCamera);
  }

  SerialCam.write(messToCamera, sizeof(messToCamera));
}

void logMessage(uint8_t hexString[]){
  for(int i=0; i<7; i++){
    Serial.printf("%02X", hexString[i]);
    Serial.print(" ");
  }
  Serial.println();
}