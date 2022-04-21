#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoCam MyPelcoCam(01,       //Address of the camera
                    PELCO_D9600,//Config
                    7,         //RX pin (Cam to Arduino)
                    6,         //TX pin (Arduino to Cam)
                    true,       //Enable logging?
                    8);        // RE pin for manual switching modules set to -1 if it is a auto module (like a groove one)

void setup() {
    Serial.begin(9600);

    //Begin the serial communication
    MyPelcoCam.begin(); 

    //Send stop, send_command will return a bool that indicates if the cameras sended its ACK
    //It's a good way to see if the camera is plugged in
    while (!MyPelcoCam.send_command(STOP)) { 
        Serial.println("Camera not plugged?");
        delay(1000);
    }
    Serial.println("Init Finished!");
}

void loop() {
    //loops the camera increment angle of 1°; very fun because it shakes like hell
      int i=0;
      for(i=0; i<35999; i=i+100){
         MyPelcoCam.send_command(SET_PAN, i); //Hundered of degrees, max is 35999 == 359,99°
         delay(500);
      }

}