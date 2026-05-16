#include "tick_test_utils.h"

#include <cstring>

// writing uint64_t in little-endian byte order

// example:
// value = 0x1122334455667788
// output bytes = 88 77 66 55 44 33 22 11
static void write_u64_le(uint8_t* out, uint64_t value) {
    for (int i = 0; i < 8; i++) {
        out[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFFu);
    }
}

// writing uint32_t in little-endian byte order
static void write_u32_le(uint8_t* out, uint32_t value) {
    for (int i = 0; i < 4; i++) {
        out[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFFu);
    }
}

void encode_tick_20(const Tick& tick, uint8_t out[TICK_DISK_SIZE]) {
    // bytes 0:7 = timestamp
    write_u64_le(out, tick.time);

    // bytes 8:15 = double price
    // copy double into uint64_t to byte by byte
    uint64_t price_bits = 0;
    std::memcpy(&price_bits, &tick.price, sizeof(double));
    write_u64_le(out + 8, price_bits);

    // bytes 16:19 = volume
    write_u32_le(out + 16, tick.volume);
}

std::vector<uint8_t> encode_tick_payload(const std::vector<Tick>& ticks) {
    // one Tick is 20 bytes on disk
    std::vector<uint8_t> payload;
    payload.resize(ticks.size() * TICK_DISK_SIZE);

    for (size_t i = 0; i < ticks.size(); i++) {
        uint8_t* output_location = payload.data() + (i * TICK_DISK_SIZE);
        encode_tick_20(ticks[i], output_location);
    }

    return payload;
}