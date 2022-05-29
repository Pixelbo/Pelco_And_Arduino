#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoBus MyPelcoBus(6,  // TX pin (Arduino to Cam)
                    7,  // RX pin (Cam to Arduino)
                    8); // RE pin for manual switching modules set to -1 if it is a auto module (like a groove one)

void setup() {
    //Here we don't need to declare a camera because all we send is raw

    Serial.begin(9600);

    // Begin the serial communication
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