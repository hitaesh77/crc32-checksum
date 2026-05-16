#include <cstdint>
#include <array>
#include <iostream>
#include "checksum.h"

// reflected CRC32 polynomial.
// common CRC32 version used by zlib, Ethernet, zip, etc.
static constexpr uint32_t POLYNOMIAL = 0xEDB88320u;

// ---------------------------------------------------------------
// NAIVE CHECKSUM IMPLEMENTATION
// ---------------------------------------------------------------

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

// ---------------------------------------------------------------
// TABLED CHECKSUM IMPLEMENTATION
// ---------------------------------------------------------------
static constexpr uint32_t compute_crc32_table_entry(uint32_t byte) {
    uint32_t crc = byte;

    for (int bit = 0; bit < 8; bit++) {
        if (crc & 1u) {
            crc = (crc >> 1) ^ POLYNOMIAL;
        } else {
            crc >>= 1;
        }
    }

    return crc;
}

static constexpr std::array<uint32_t, 256> initialize_crc32_table() {
    std::array<uint32_t, 256> table = {};

    for (uint32_t i = 0; i < table.size(); i++) {
        table[i] = compute_crc32_table_entry(i);
    }

    return table;
}

// global crc32 table
static constexpr std::array<uint32_t, 256> CRC32TABLE = initialize_crc32_table();

uint32_t compute_checksum_table(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFFu;

    for (size_t i = 0; i < length; i++) {
        uint8_t table_index = static_cast<uint8_t>((crc ^ data[i]) & 0xFFu);

        crc = (crc >> 8) ^ CRC32TABLE[table_index];
    }

    return crc ^ 0xFFFFFFFFu;
}