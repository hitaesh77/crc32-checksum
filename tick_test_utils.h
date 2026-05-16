#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// struct Tick model for testing.
// in TickDB project, will be checksuming exact bytes written to WAL
// helpers convert Tick values into the 20-byte WAL payload format

// struct used in TickDB
struct Tick {
    uint64_t time;
    double price;
    uint32_t volume;
};

// used ocnstexpr to force this during compile time and not runtime
static constexpr size_t TICK_DISK_SIZE = 20;

// encode a single tick struct
void encode_tick_20(const Tick& tick, uint8_t out[TICK_DISK_SIZE]);

// encode a batch of tick structs to be written to WAL
std::vector<uint8_t> encode_tick_payload(const std::vector<Tick>& ticks);