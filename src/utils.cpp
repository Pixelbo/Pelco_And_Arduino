#include "Arduino.h"
#include "utils.h"


/*!
 *  @brief  Search a value trough a array of bytes one function is for progmem the other one isn't for progmem
 *
 *  @param  look_array the array
 *  @param  value value to lookup
 *
 * @return the index of the element found
 */

int Utils::searchIndexPROGMEM(const byte look_array[], byte value) {
    int i = 0;
    for (i = 0; i <= (sizeof(look_array) / sizeof(*look_array)); i++) {
        if (pgm_read_byte(&look_array[i]) == value) {
            return i;
        }
    }
    return -1;
}

// For an x or y reason sizeof don't work properly
int Utils::searchIndex(byte look_array[], byte value, size_t size) {

    int i = 0;
    for (i = 0; i <= (size); i++) {

        if (look_array[i] == value) {
            return i;
        }
    }
    return -1;
}