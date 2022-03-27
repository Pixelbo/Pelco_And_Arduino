#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoCam PelcoCam(0x01,       //Address of the camera
                PELCO_D9600,  //Config
                32,           //TX pin
                33,           //RX pin
                true,         //Enable logging?
                25);         //RE pin for manual switching modules set to -1 if it is a auto module (like a groove one)

void setup() {
    //Begin the serial communication If you enabled logging, no need to    begin the normal serial
    PelcoCam.begin(); 

    //Send stop, send_command will return a bool that indicates if the cameras sended its ACK
    //It's a good way to see if the camera is plugged in
    while (!PelcoCam.send_command(STOP)) { 
        Serial.println("Camera not plugged?");
        delay(1000);
    }
    Serial.println("Init Finished!");
}

void loop() {

    //Pan to the left with max speed before turbo
    PelcoCam.send_command(PAN_L, 0x3F); 
    delay(1000);
    PelcoCam.send_command(STOP); //Stop the camera
    delay(1000);
    //Pan to the left with less speed than the tilt speed that goes up
    PelcoCam.send_comand(PAN_L_TILT_U, 0x10, 0xFF);)
    delay(1000);
    PelcoCam.send_command(STOP); //Stop the camera
    delay(1000);
}