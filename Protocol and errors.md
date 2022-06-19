# Here is some explanation for the errors that you can get:

When you send some data, the frame is like this:

    uint8_t messToCamera[7] = {
        0xFF, // sync byte
        0x01, // Address
        0x00, // Always 0 (in most of the cases)
        0x02, // Command
        0x00, // Data1 (Used for pan speed and response from camera)
        0x3F, // Data2 (Used everywhere)
        0x42  // Checksum: add all byte except sync and then modulo 0x100
    };

When you receive ACK from the camera, it's like this:

    uint8_t ACKmessFromCamera[4] = {
        0xFF, //sync byte
        0x01, //Adress
        0x00, // 0?
        0x42  //Checksum taken for the previous message
    };

When you receive data from the camera (see get_position), it's like this:

    uint8_t messFromcamera[7] = {
        0xFF, // sync byte
        0x01, // Address
        0x00, // Always 0 (in most of the cases)
        0x59, // Respond to the query command
        0x20, // Data MSB
        0x15, // DATA LSB
        0x8F  // Checksum: add all byte except sync and then modulo 0x100
    };


## So here are the errors you might encounter:

### Acknoledge errors

    "Cam 1: ERROR Could not verify camera ACK: timeout reached (is camera well plugged in?)\n"

The acknoledge took too long to be received, maybe the camera isn't plugged in.
Default timeout is 10 ms.

    "Cam 1: ERROR Could not verify camera ACK: bad sync byte (is camera well plugged in?)\n",

The acknoledge data that you received isn't right, the sync byte isn't detect by the program

    "Cam 1: ERROR Could not verify camera ACK: bad address (is camera well plugged in?)\n",

The acknoledge data that you received isn't right, the address byte isn't the one from the camera

    "Cam 1: ERROR Could not verify camera ACK: bad null ???? (is camera well plugged in?)\n",

The acknoledge data that you received isn't right, the null byte (the third one) isn't detected by the program

    "Cam 1: ERROR Could not verify camera ACK: bad checksum (is camera well plugged in?)\n",

The acknoledge data that you received isn't right, the checksum byte isn't good

### Request errors

    "Cam 1: No valid request provided\n"
    
The command you provided isn't listed as a request (valid request are: QUERY_PAN, QUERY_TILT, QUERY_ZOOM)

    "Cam 1: ERROR timout reached\n"

Same error as the acknoledge timeout

    "Cam 1: Warn: no reponse from camera (bad index)\n"

The data received from the camera isn't properly "aligned", the program seek for the respond command and throw
this error if it isn't at the fourth byte.