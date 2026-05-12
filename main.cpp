#include <iostream>
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
    struct tick test = { 1234567890, 1.23, 10 };
    
    // Compute the CRC32 checksum for the tick structure
    uint32_t checksum = compute_checksum_naive(&test, sizeof(test));

    printf("CRC32: %08X\n", checksum);

    return 0; 
}