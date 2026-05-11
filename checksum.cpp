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

// NOT OPTIMIZED
// update crc value with new bit
uint32_t update_crc_naive(uint32_t crc, uint32_t bit) {
    // XOR with a predefined polynomial if the high-order bit of the shifted CRC is 1
    if (bit) {
        crc ^= POLYNOMIAL;
    }
    // XOR with the current byte, also shifting each bit left
    return (crc << 1) ^ 0xFFFFFFFF;
}

// go thorugh each byte of the data and compute crc for that byte
uint32_t compute_byte_naive(uint32_t crc, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        crc = update_crc_naive(crc, (byte >> (7 - i)) & 1);
    }

    return crc;
}

// compute final checksum given data of any length (Tick struct here so 20 bytes)
uint32_t compute_checksum_naive(const void* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;

    const uint8_t* bytes = (const uint8_t*) data; // type cast data to be treated as array of bytes
    for (int i = 0; i < length; i++) {
        crc = compute_byte_naive(crc, bytes[i]);
    }

    // XOR with all 1s in the end
    return crc ^ 0xFFFFFFFF;
}

// OPTIMIZED