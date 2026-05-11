#include <cstdint>

static uint8_t checksum_table[256];
static bool checksum_table_intialized = false;

void initialize_checksum_table(void);
uint32_t compute_checksum(const void* data, size_t size);

// NAIVE CHECKSUM
// NON OPTIMIZED
uint32_t update_crc_naive(uint32_t crc, uint32_t bit);
uint32_t compute_byte_naive(uint32_t crc, uint8_t byte);
uint32_t compute_checksum_naive(const void* data, size_t length);
