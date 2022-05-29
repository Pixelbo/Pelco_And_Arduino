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
    Camera1.disable_ack = false; //For this exemple, this must set to false!


    // Send stop, send_command will return a bool that indicates if the cameras sended its ACK
    // It's a good way to see if the camera is plugged in
    while (!MyPelcoBus.send_command(Camera1, STOP)) {
        Serial.println("Camera not plugged?");
        delay(1000);
    }
    Serial.println("Init Finished!");
}

void loop() {
        MyPelcoBus.send_command(Camera1, PAN_L, 0x3F); // Spin for a random time
        delay(random(1000, 2000));

        int angle100 = MyPelcoBus.send_request(Camera1, QUERY_PAN); // Get the current angle of the camera

        float angle = angle100 / 100.0; // Convert to float

        Serial.println("Angle detected: " + String(angle));
}
