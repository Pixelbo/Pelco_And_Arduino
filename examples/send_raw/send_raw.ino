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
    while(Serial.available()) {
        String in = Serial.readString(); //Read from the serial
        PelcoCam.send_raw(in); //Send the raw command to the cameras (adress can be another than the one declared in the object because it's raw transmission)
        delay(500);
        PelcoCam.send_command(STOP); //Stop it after a while
    }
}