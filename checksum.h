#include <cstdint>

static uint8_t checksum_table[256];
static bool checksum_table_intialized = false;

void initialize_checksum_table(void);
uint32_t compute_checksum(const void* data, size_t size);