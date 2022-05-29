#include "Arduino.h"
#include <Pelco_And_Arduino.h>

PelcoBus MyPelcoBus(6,  // TX pin (Arduino to Cam)
                    7,  // RX pin (Cam to Arduino)
                    8); // RE pin for manual switching modules set to -1 if it is a auto module (like a groove one)

PelcoBus::PelcoCam Camera1; //Declare a camera

void setup() {
    Serial.begin(9600);

    // Begin the serial communication
    MyPelcoBus.begin(PELCO_D9600, true);

    // Set camera settings
    Camera1.address = 1; //Address of the camera
    Camera1.disable_ack = true; //Old models function one 2 pair of wires and only one is usable, the TX one, so it's impossible that it can send back a reqponse


    // Send stop, send_command will return a bool that indicates if the cameras sended its ACK
    // It's a good way to see if the camera is plugged in
    while (!MyPelcoBus.send_command(Camera1, STOP)) {
        Serial.println("Camera not plugged?");
        delay(1000);
    }
    Serial.println("Init Finished!");
}

void loop() {

    //Pan to the left with max speed before turbo
    MyPelcoBus.send_command(Camera1, PAN_L, 0x3F);
    delay(1000);
    MyPelcoBus.send_command(Camera1, STOP); //Stop the camera
    delay(1000);
    //Pan to the left with less speed than the tilt speed that goes up
    MyPelcoBus.send_command(Camera1, PAN_R, 0x3F);
    delay(1000);
    MyPelcoBus.send_command(Camera1, STOP); //Stop the camera
    delay(1000);
}