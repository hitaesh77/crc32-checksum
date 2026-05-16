#include <cstdint>
#include "checksum.h"

// reflected CRC32 polynomial.
// common CRC32 version used by zlib, Ethernet, zip, etc.
#define POLYNOMIAL 0xEDB88320

// NAIVE CHECKSUM IMPLEMENTATION

// treat data as an array of bytes 
uint32_t compute_checksum_naive(const uint8_t* data, size_t length) {
    // step 1: start with all bits flipped
    uint32_t crc = 0xFFFFFFFFu;

    // step 2: loop through every byte of data passed in
    for (size_t i = 0; i < length; i++) {

        // step 2.1: mix in curr byte of data with lower byte of crc
        crc ^= data[i];

        // step 2.2: go thorugh every bit of that byte
        for (int bit = 0; bit < 8; bit++) {

            // step 2.3: if lowest bit set, shift left and mix with reversed polynomial
            if (crc & 1) {
                crc = (crc >> 1) ^ POLYNOMIAL;
            } else {
                crc >>= 1;
            }
        }
    }

    // step 3: flip all bits
    return crc ^ 0xFFFFFFFFu;
}
