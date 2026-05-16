#pragma once

#include <cstddef>
#include <cstdint>

// naive CRC32 implementation
// the slow, bit-by-bit calculation
uint32_t compute_checksum_naive(const uint8_t* data, size_t length);

// Future implementations can go here later.
// uint32_t compute_checksum_table(const uint8_t* data, size_t length);
// uint32_t compute_checksum_fast(const uint8_t* data, size_t length);