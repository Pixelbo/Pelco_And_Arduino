#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoBus MyPelcoBus(6,  // RX pin (Cam to Arduino)
                    7,  // TX pin (Arduino to Cam)
                    8); // RE pin for manual switching modules set to -1 or nothing if it is a auto module (like a groove one)

    
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
        Camera1.send_command(SET_PAN, random(0, 35999)); // set pan to a random angle

        int angle100 = Camera1.send_request(QUERY_PAN); // Get the current angle of the camera

        float angle = angle100 / 100.0; // Convert to float

        Serial.println("Angle detected: " + String(angle));
        delay(200);
}
