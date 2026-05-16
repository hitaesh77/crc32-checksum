#include <iostream>
#include <cassert>
#include <cstdio>
#include "checksum.h"

// Checksum Testing!

// Tick struct for testing (20 bytes)
#pragma pack(push, 1)
struct tick {
    uint64_t time;
    double price;
    uint32_t volume;
};
#pragma pack(pop)


int main(int argc, char** argv) {
    // TESTING NAIVE CHECKSUM

    // Test 1:
    std::string test = "123456789";
    struct tick tick_test = { 1234567890, 1.23, 10 };

    assert(sizeof(tick_test) == 20 && "Test 1: FAILED - tick struct should be 20 bytes");
    std::cout << "Test 1: PASSED - tick struct is 20 bytes" << std::endl;
    
    // Compute the CRC32 checksum for the tick structure
    uint32_t checksum = compute_checksum_naive((const uint8_t*)(&tick_test), sizeof(tick_test));
    uint32_t checksum_string = compute_checksum_naive((const uint8_t*)(test.data()), test.size());

    printf("CRC32 struct: %08X\n", checksum);
    printf("CRC32 string: %08X\n", checksum_string);

    return 0; 
}