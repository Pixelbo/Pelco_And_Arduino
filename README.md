# Pelco and arduino

> Photography has nothing to do with cameras.

![A Pelco Camera](the%20eye%20of%20sauron.jpg)

## Overview

Hello, this is an Arduino library designed to control Pelco Cameras!

For now it can support PELCO-D protocol but I havn't got tested everything yet.

This library was made for the project: [Controlling a cctv camera on Arduino](https://hackaday.io/project/183986-controlling-a-cctv-camera-with-arduino) on hacakday.io

Before you go on, this is a school, project, I'm still in school and learnings things so be kind if the libray is ugly. I would like to thanks my school and teachers for allowing me to work on this project.

The main goal of the project is to do a control table (It'll be on arduino.)

Anyway, here the table of contents:

## Table of Contents
- [Pelco and arduino](#pelco-and-arduino)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [Requirements](#requirements)
  - [Exemple](#exemple)


## Requirements

This is a bit obvious but you'll need a Pelco camera, the one I use the ES4036.

The important thing is to set the camera is RS485 biderectional and with the baud of your chossing. For now, the lib only support Pelco-D protocol.

About the modules for RS485, I'm working with a standart MAX485 chip but I'll test a more advanced module on the future.

## Exemple

```arduino
    #include "Arduino.h"
    #include <Pelco_And_Arduino.h>

    int Pan_position = 0;

    PelcoCam PelcoCam(0x01,       //Address of the camera
                    PELCO_D9600,//Config
                    32,         //TX pin
                    33,         //RX pin
                    true,       //Enable logging?
                    25);        //RE pin for manual switching modules

    void setup() {

        //Begin the serial communication If you enabled logging, no need to    begin the normal serial
        PelcoCam.begin(); 

        //Send stop, send_command will return a bool that indicates if the cameras sended its ACK
        while (!PelcoCam.send_command(STOP)) { 
            Serial.println("Camera not plugged?");
            delay(1000);
        }
        Serial.println("Init Finished!");
    }
    void loop() {
    
        //Pan to the left with max speed before turbo
        PelcoCam.send_command(PAN_L, 0x3F); 

        //send a request about the pan position
        Pan_position = PelcoCam.send_request(QUERY_PAN);   
        Serial.println(Pan_position);

        delay(1000);
        
        PelcoCam.send_command(STOP); //Stop the camera
        delay(1000);
    }
```