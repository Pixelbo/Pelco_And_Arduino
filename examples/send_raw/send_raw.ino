#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoCamBus MyPelcoBus(6,  // TX pin (Arduino to Cam)
                       7,  // RX pin (Cam to Arduino)
                       8); // RE pin for manual switching modules set to -1 if it is a auto module (like a groove one)

void setup() {

    Serial.begin(9600);

    // Begin the serial communication
    MyPelcoBus.begin(PELCO_D9600, true);

    // Send stop, send_command will return a bool that indicates if the cameras sended its ACK
    // It's a good way to see if the camera is plugged in
    while (!MyPelcoBus.send_command(1, STOP)) {
        Serial.println("Camera not plugged?");
        delay(1000);
    }
    Serial.println("Init Finished!");
}

void loop() {
    while(Serial.available()) {
        String in = Serial.readString(); //Read from the serial
        MyPelcoBus.send_raw(in); //Send the raw command to the cameras (adress can be another than the one declared in the object because it's raw transmission)
        delay(500);
        MyPelcoBus.send_command(1, STOP); //Stop it after a while
    }
}