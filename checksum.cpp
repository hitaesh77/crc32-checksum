#include <cstdint>
#include <iostream>
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

// uint32_t compute_checksum_naive(const void* data, size_t length) {
//     uint32_t crc = 0xFFFFFFFF; // starting CRC

//     const uint8_t* bytes = (const uint8_t*) data; // type cast data to be treated as array of bytes

//     // iterate through bytes
//     for (int i = 0; i < length; i++) {
//         uint32_t byte = bytes[i]; // isolate the current byte

//         // iterate through bits of byte
//         // for (int j = 7; j >= 0; --j) {
//         //     uint32_t bit = (byte >> j) & 1; // isolate curr bit
//         //     crc = (crc << 1);
//         //     if (bit) {
//         //         crc ^= POLYNOMIAL;
//         //     }
//         // }
//         for (int j = 0; j < 8; j++) {
//             // uint32_t bit = (byte << j) & 0x80000000;
//             uint32_t bit = (byte >> j) & 1; // Isolate curr bit
//             crc <<= 1;
//             if (bit) {
//                 crc ^= POLYNOMIAL;
//             }
//         }
//     }

//     // XOR with all 1s in the end
//     return crc ^ 0xFFFFFFFF;
// }

uint32_t compute_checksum_naive(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF; // Initial value for CRC-32

    for (size_t i = 0; i < length; ++i) {
        printf("iter #: %d ", i);
        printf("current byte: %02X ", data[i]);

        crc ^= (data[i] << 24); // XOR the current byte with crc
        printf("curr iter crc: %08X \n", crc);
        for (int j = 0; j < 8; ++j) { // Process bits from MSB to LSB
            if ((crc & (1 << 31)) == 1) { // Check the most significant bit
                crc = (crc >> 1) ^ POLYNOMIAL;
            } else {
                crc >>= 1; // Shift left by 1 bit
            }
        }
    }
    
    printf("final before return crc: %08X \n", crc); 

    return crc ^ 0xFFFFFFFF; // Final XOR to get the checksum value
}

// uint32_t compute_checksum_naive(const uint8_t* data, size_t length) {
//     uint32_t crc = 0xFFFFFFFF; // Initial value for CRC-32
//     for (size_t i = 0; i < length; ++i) {
//         uint8_t byte = data[i];
//         for (int j = 7; j >= 0; --j) { // Process bits from MSB to LSB
//             crc <<= 1;
//             if ((byte & (1 << j)) != 0) {
//                 crc ^= POLYNOMIAL;
//             }
//         }
//     }
//     return crc ^ 0xFFFFFFFF; // Final XOR to get the checksum value
// }