#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// function pointer type for any CRC32 implementation
// allows using same function for different crc implementations, inclusing libraries
using crcFunc = uint32_t (*)(const uint8_t* data, size_t length);

struct crcImpl {
    const char* name;
    crcFunc function;
};

struct testStats {
    int tests_run = 0;
    int tests_failed = 0;
};

// expect equal
void expect_eq(
    testStats& stats,
    const char* impl_name,
    const char* test_name,
    uint32_t expected,
    uint32_t actual
);

// epxect not equal
void expect_ne(
    testStats& stats,
    const char* impl_name,
    const char* test_name,
    uint32_t left,
    uint32_t right
);

void print_test_summary(const testStats& stats);