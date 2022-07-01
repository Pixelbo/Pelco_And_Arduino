#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoBus MyPelcoBus(6,  // RX pin (Cam to Arduino)
                    7,  // TX pin (Arduino to Cam)
                    8); // RE pin for manual switching modules set to -1 or nothing if it is a auto module (like a groove one)


//No need to declare a camera here

void setup() {
    Serial.begin(9600);

    // Begin the bus communication at 9600 and enable logging
    MyPelcoBus.begin(PELCO_D9600, true);

    Serial.println("Init Finished!");
}

void loop() {
    while(Serial.available()) {
        String in = Serial.readString(); //Read from the serial
        MyPelcoBus.send_raw(in); //Send the raw command to the cameras (adress can be another than the one declared in the object because it's raw transmission)
        //You will need to stop it manually
        // Very simple: just send this commnad : "FF *address* 00 00 00 00 01"
    }
}