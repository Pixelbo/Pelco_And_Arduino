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
    //loops the camera increment angle of 1°; very fun because it shakes like hell
      for(int i=0; i<35999; i=i+100){
         Camera1.send_command(SET_PAN, i); //Hundered of degrees, max is 35999 == 359,99°
         delay(500);
      }

}