#include <cstdint>
#include "checksum.h"

#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

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