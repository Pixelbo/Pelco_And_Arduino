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

#include "Arduino.h"
#include <SoftwareSerial.h>

#include "Pelco_And_Arduino.h"

/*!
 *  @brief  Constructor
 * 
 *  @param  Address The address of the camera
 *  @param  baud baud of the cameras
 *  @param parameter-name description
 * 
 */

PelcoCam::PelcoCam(uint8_t Address, int baud, int txPin, int rxPin, bool log_messages=false){
    Address_ = Address;
    baud_ = baud;
    txPin_ = txPin;
    rxPin_ = rxPin;
    log_messages_ = log_messages;

}

/*!
 *  @brief  begin everything
 * 
 */

void PelcoCam::begin(){
    if(log_messages_){
        Serial.begin(115200);
        Serial.println("Message log has been activated!");
    }

    SerialCam.begin(baud_, SWSERIAL_8N1, rxPin_, txPin_);
}

/*!
 *  @brief  Send message to the camera
 * 
 *  @param  command the wanted command (see header)
 *  @param  params Main parameter
 *  @param  params2 Second parameter for command that requires 2 parameters
 * 
 */

void PelcoCam::send_command(uint8_t command, uint8_t params, uint8_t params2){
  messToCamera[0] = 0xFF;
  messToCamera[1] = Address_;

  if(command == ON || command == OFF || command == FOCUS_N){ //If the command is on or off, set the command to uint8_t 3
      messToCamera[2] = command;
      messToCamera[3] = 0x00;
  }else{
      messToCamera[2] = 0x00;
      messToCamera[3] = command;
  }

  if(command == PAN_L || command == PAN_R){
      messToCamera[4] = params;
      messToCamera[5] = 0x00;
  }else if(command == PAN_L_TILT_U || command == PAN_R_TILT_U || command == PAN_L_TILT_D || command == PAN_R_TILT_D){
      messToCamera[4] = params2;
      messToCamera[5] = params;
  }else{
      messToCamera[4] = 0x00;
      messToCamera[5] = params;
  }

  messToCamera[6] = (messToCamera[1] + messToCamera[2] + messToCamera[3] +messToCamera[4] +messToCamera[5])%0x100; //Checksum

  if(log_messages_){
    Serial.print("Sending message: ");
    for(int i=0; i<7; i++){
      Serial.printf("%02X", messToCamera[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  SerialCam.write(messToCamera, sizeof(messToCamera));
}

/*!
 *  @brief  Send a query to the camera and reads the response
 * 
 *  @param  request the wanted reponse (see header)
 *  @param  timeout default 1000; timout for waiting a byte
 *  @param  maxbuffer Maximum size of the buffer; defaut 20
 * 
 * @return true if succeded, false if an error occured
 */

bool PelcoCam::send_request(uint8_t request, uint timeout, uint maxbuffer){
    byte response_command;

    if(request == QUERY_PAN){response_command == RESP_PAN;}
    else if(request == QUERY_TILT){response_command = RESP_TILT;}
    else if(request == QUERY_ZOOM){response_command = RESP_ZOOM;}
    else if(request == QUERY_FOCUS){response_command = RESP_FOCUS;}
    else{
        if(log_messages_) Serial.println("No valid request provided");
        return false;
    }

    send_command(request); //Send the query

    while (!SerialCam.available()){//Wait for the first bit
        if (timeout==0){ //If timeout is reached
            if(log_messages_) Serial.print("ERROR timout reached");
            return false;
        }
        timeout--;
        delayMicroseconds(10);
    }

    byte buffer[maxbuffer]; // Buffer for the reception from the camera
    uint index = 0;

     while(SerialCam.available()){//Do it until there is no more things to read
        buffer[index]=SerialCam.read();
        index++;
     } 

    /* A theorical response:
    FF   00   59      4A  13  B7
    sync null command msb lsb checksum

    A practical response (what does the arduino reads):
        FF 00 FF 00 59 4A 13 B7 FF 01 48
    */

    int command_index = searchIndex(buffer, 0x59);//Looks up where is the index of the response command

    if(command_index == -1 //Checks if found
    || command_index < 3  //Checks if the reponse byte is in the right place
    ){
        if(log_messages_)Serial.println("Warn: no reponse from camera");
        return false;
    }

    //Checksum is sum of everything except sync modulo 0x100
    bool checksum = ((buffer[command_index] + buffer[command_index+1] + buffer[command_index+2])%0x100 == buffer[command_index+3]);
    
    //Checking the sync byte, the null byte and the checksum
    if(buffer[command_index-3] != 0xFF && 
       buffer[command_index-2 != 00] &&
       !checksum){
        return false;
    }
    
    for(int i=0; i<7; i++){
        messFromcamera[i] = buffer[(command_index-3)+i];
    }

    if(log_messages_){
        Serial.print("Message from camera: ");
        for(int i=0; i<7; i++){
            Serial.printf("%02X ", messFromcamera[i]);
        } 
        Serial.println();
    }

    return true;
}

/*!
 *  @brief  Dearch a value trough a array of bytes
 * 
 *  @param  look_array the array
 *  @param  value value to lookup
 * 
 * @return the index of the element found
 */

int PelcoCam::searchIndex(byte look_array[], byte value){
  for(int i=0; i<sizeof(look_array)/sizeof(look_array[0]); i++){
    if (look_array[i] == value){
      return i;
    }
  }
  return -1;
}