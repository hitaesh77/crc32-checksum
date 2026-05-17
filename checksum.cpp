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

// ---------------------------------------------------------------
// 4-SLICE CHECKSUM IMPLEMENTATION
// ---------------------------------------------------------------
static constexpr std::array<std::array<uint32_t, 256>, 4> initialize_crc32_table_4slice() {
    std::array<std::array<uint32_t, 256>, 4> table = {};

    // first table is the normal CRC32 table.
    for (uint32_t i = 0; i < 256; i++) {
        table[0][i] = compute_crc32_table_entry(i);
    }

    // table[1] = effect after one more byte position
    // table[2] = effect after two more byte positions
    // table[3] = effect after three more byte positions
    for (int slice = 1; slice < 4; slice++) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t previous = table[slice - 1][i];
            table[slice][i] = (previous >> 8) ^ table[0][previous & 0xFFu];
        }
    }

    return table;
}

// global 4slice crc32 table
static constexpr std::array<std::array<uint32_t, 256>, 4> CRC32TABLE_4SLICE = initialize_crc32_table_4slice();

// safely read 4 bytes from memory as little-endian uint32_t
static inline uint32_t read_u32_4bytes_le(const uint8_t* data) {
    return static_cast<uint32_t>(data[0]) |
        (static_cast<uint32_t>(data[1]) << 8) |
        (static_cast<uint32_t>(data[2]) << 16) |
        (static_cast<uint32_t>(data[3]) << 24);
}

uint32_t compute_checksum_four_slice(const uint8_t* data, size_t length){
    uint32_t crc = 0xFFFFFFFFu;

    // process 4 bytes per loop
    while (length >= 4) {
        // mix next 4 bytes into the CRC.
        crc ^= read_u32_4bytes_le(data);

        // use 4 lookup tables to process these 4 bytes in one step.
        // replaces 4 separate iterations of the normal table algorithm.
        crc =
            CRC32TABLE_4SLICE[3][(crc      ) & 0xFFu] ^
            CRC32TABLE_4SLICE[2][(crc >>  8) & 0xFFu] ^
            CRC32TABLE_4SLICE[1][(crc >> 16) & 0xFFu] ^
            CRC32TABLE_4SLICE[0][(crc >> 24) & 0xFFu];

        data += 4;
        length -= 4;
    }

    // handle any leftover bytes
    while (length > 0) {
        uint8_t table_index = static_cast<uint8_t>((crc ^ *data) & 0xFFu);
        crc = (crc >> 8) ^ CRC32TABLE_4SLICE[0][table_index];

        data++;
        length--;
    }

    return crc ^ 0xFFFFFFFFu;
}

// ---------------------------------------------------------------
// 8-SLICE CHECKSUM IMPLEMENTATION
// ---------------------------------------------------------------
static constexpr std::array<std::array<uint32_t, 256>, 8> initialize_crc32_table_8slice() {
    std::array<std::array<uint32_t, 256>, 8> table = {};

    // first table is the normal CRC32 table.
    for (uint32_t i = 0; i < 256; i++) {
        table[0][i] = compute_crc32_table_entry(i);
    }

    // table[1] = effect after one more byte position
    // table[2] = effect after two more byte positions
    // table[3] = effect after three more byte positions
    for (int slice = 1; slice < 8; slice++) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t previous = table[slice - 1][i];
            table[slice][i] = (previous >> 8) ^ table[0][previous & 0xFFu];
        }
    }

    return table;
}

// global 8-slice crc32 table
static constexpr std::array<std::array<uint32_t, 256>, 8> CRC32TABLE_8SLICE = initialize_crc32_table_8slice();

// safely read 4 bytes from memory as little-endian uint32_t
static inline uint64_t read_u64_8bytes_le(const uint8_t* data) {
    return static_cast<uint64_t>(data[0])      |
        (static_cast<uint64_t>(data[1]) << 8)  |
        (static_cast<uint64_t>(data[2]) << 16) |
        (static_cast<uint64_t>(data[3]) << 24) |
        (static_cast<uint64_t>(data[4]) << 32) |
        (static_cast<uint64_t>(data[5]) << 40) |
        (static_cast<uint64_t>(data[6]) << 48) |
        (static_cast<uint64_t>(data[7]) << 56);
}

uint32_t compute_checksum_eight_slice(const uint8_t* data, size_t length){
    uint64_t crc = 0xFFFFFFFFu;

    while (length >= 8) {
        crc ^= read_u64_8bytes_le(data);

        crc =
            CRC32TABLE_8SLICE[7][(crc      ) & 0xFFu] ^
            CRC32TABLE_8SLICE[6][(crc >>  8) & 0xFFu] ^
            CRC32TABLE_8SLICE[5][(crc >> 16) & 0xFFu] ^
            CRC32TABLE_8SLICE[4][(crc >> 24) & 0xFFu] ^
            CRC32TABLE_8SLICE[3][(crc >> 32) & 0xFFu] ^
            CRC32TABLE_8SLICE[2][(crc >> 40) & 0xFFu] ^
            CRC32TABLE_8SLICE[1][(crc >> 48) & 0xFFu] ^
            CRC32TABLE_8SLICE[0][(crc >> 56) & 0xFFu];

        data += 8;
        length -= 8;
    }

    // handle any leftover bytes
    while (length > 0) {
        uint8_t table_index = static_cast<uint8_t>((crc ^ *data) & 0xFFu);
        crc = (crc >> 8) ^ CRC32TABLE_8SLICE[0][table_index];

        data++;
        length--;
    }

    return crc ^ 0xFFFFFFFFu;
}