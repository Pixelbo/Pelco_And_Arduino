#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoBus MyPelcoBus(6,  // RX pin (Cam to Arduino)
                    7);  // TX pin (Arduino to Cam)

    
PelcoCam Camera1(&MyPelcoBus, //The pointer to the bus
                 1,           //Address of the camera
                 false);      //We are getting a response

void setup() {
    Serial.begin(9600);

    // Begin the bus communication at 9600 and enable logging
    MyPelcoBus.begin(PELCO_D9600, true);

    //Is the camera online?
    while (!Camera1.available()) {
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