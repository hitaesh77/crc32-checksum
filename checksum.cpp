#include <cstdint>
#include "checksum.h"

#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

#define POLYNOMIAL 0xEDB88320

void initialize_checksum_table(void) {
    uint8_t remainder = 0;

    for (int dividend = 0; dividend < 256; dividend++) {
        remainder = dividend << (WIDTH - 8);

        for (uint8_t bit = 8; bit > 0; bit--) {

        }
    }
}

uint32_t compute_checksum(const void* data, size_t size) {
    return 0;
}


// NAIVE CHECKSUM IMPLEMENTATION

uint32_t compute_checksum_naive(const void* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF; // starting CRC

    const uint8_t* bytes = (const uint8_t*) data; // type cast data to be treated as array of bytes

    // iterate through bytes
    for (int i = 0; i < length; i++) {
        uint32_t byte = bytes[i]; // isolate the current byte

        // iterate through bits of byte
        for (int j = 7; j >= 0; --j) {
            uint32_t bit = (byte >> j) & 1; // isolate curr bit
            crc = (crc << 1) ^ (bit ? POLYNOMIAL : 0); // XOR with polynomial if curr bit high
        }
    }

    // XOR with all 1s in the end
    return crc ^ 0xFFFFFFFF;
}
