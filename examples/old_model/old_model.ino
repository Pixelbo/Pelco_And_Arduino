#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoBus MyPelcoBus(6,  // RX pin (Cam to Arduino)
                    7,  // TX pin (Arduino to Cam) 
                    8); // RE pin for manual switching modules set to -1 or nothing if it is a auto module (like a groove one)

////IMPORTANT: Even if it is an old model without returning a response capabilities you need to specify a RX pin and a RE pin
    
PelcoCam Camera1(&MyPelcoBus, //The pointer to the bus
                 1,           //Address of the camera
                 true);      //We are NOT getting a response

void setup() {
    Serial.begin(9600);

    // Begin the bus communication at 9600 and enable logging
    MyPelcoBus.begin(PELCO_D9600, true);

    //Is the camera online? this will fail because it is an old models
    while (Camera1.available()/* <---- will fail! */) { 
        Serial.println("Camera not plugged?");
        delay(1000);
    }
    Serial.println("Init Finished!");
}

void loop() {

    //Pan to the left with max speed before turbo
    Camera1.send_command(PAN_L, 0x3F);
    delay(1000);
    Camera1.send_command(STOP); //Stop the camera
    delay(1000);
    //Pan to the left with less speed than the tilt speed that goes up
    Camera1.send_command(PAN_R, 0x3F);
    delay(1000);
    Camera1.send_command(STOP); //Stop the camera
    delay(1000);
}