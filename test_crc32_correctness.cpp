#include "test_crc32_correctness.h"

#include "tick_test_utils.h"

#include <cstdint>
#include <string>
#include <vector>

// zlib wrapper that gives industry standard CRC32 function
#ifdef HAS_ZLIB
#include <zlib.h>

static uint32_t compute_checksum_zlib_reference(const uint8_t* data, size_t length) {
    uLong crc = crc32(0L, Z_NULL, 0);

    // zlib takes a uInt length, so this safely handles very large buffers
    // by processing the input in chunks
    static constexpr size_t MAX_CHUNK = 1u << 30;

    size_t offset = 0;

    while (offset < length) {
        size_t remaining = length - offset;

        uInt chunk_size = static_cast<uInt>(
            remaining > MAX_CHUNK ? MAX_CHUNK : remaining
        );

        crc = crc32(
            crc,
            reinterpret_cast<const Bytef*>(data + offset),
            chunk_size
        );

        offset += chunk_size;
    }

    return static_cast<uint32_t>(crc);
}
#endif

static void test_empty_input(const crcImpl& impl, testStats& stats) {
    uint32_t actual = impl.function(nullptr, 0);

    expect_eq(
        stats,
        impl.name,
        "empty input returns 00000000",
        0x00000000u,
        actual
    );
}

static void test_standard_string(const crcImpl& impl, testStats& stats) {
    // classic CRC32 test vector
    std::string input = "123456789";

    uint32_t actual = impl.function(
        reinterpret_cast<const uint8_t*>(input.data()),
        input.size()
    );

    expect_eq(
        stats,
        impl.name,
        "\"123456789\" returns CBF43926",
        0xCBF43926u,
        actual
    );
}

static void test_changed_byte(const crcImpl& impl, testStats& stats) {
    std::string original = "123456789";
    std::string changed = original;

    // change one byte in the middle, should cause diff checksum
    changed[4] = 'X';

    uint32_t crc_original = impl.function(
        reinterpret_cast<const uint8_t*>(original.data()),
        original.size()
    );

    uint32_t crc_changed = impl.function(
        reinterpret_cast<const uint8_t*>(changed.data()),
        changed.size()
    );

    expect_ne(
        stats,
        impl.name,
        "changing one byte changes CRC",
        crc_original,
        crc_changed
    );
}

static void test_truncated_input(const crcImpl& impl, testStats& stats) {
    std::string original = "123456789";
    std::string truncated = "12345678";

    uint32_t crc_original = impl.function(
        reinterpret_cast<const uint8_t*>(original.data()),
        original.size()
    );

    uint32_t crc_truncated = impl.function(
        reinterpret_cast<const uint8_t*>(truncated.data()),
        truncated.size()
    );

    expect_ne(
        stats,
        impl.name,
        "truncating one byte changes CRC",
        crc_original,
        crc_truncated
    );
}

static void test_same_input_twice(const crcImpl& impl, testStats& stats) {
    std::string input = "TickDB deterministic checksum test";

    uint32_t first = impl.function(
        reinterpret_cast<const uint8_t*>(input.data()),
        input.size()
    );

    uint32_t second = impl.function(
        reinterpret_cast<const uint8_t*>(input.data()),
        input.size()
    );

    expect_eq(
        stats,
        impl.name,
        "same input twice returns same CRC",
        first,
        second
    );
}

static void test_tick_payload_same_input_twice(
    const crcImpl& impl,
    testStats& stats
) {
    // closer to actual TickDB WAL use case
    // encode ticks into a stable 20-byte-per-tick payload first, then checksum those bytes
    std::vector<Tick> ticks = {
        {1234567890ULL, 1.23, 10},
        {1234567990ULL, 1.24, 20},
        {1234568090ULL, 1.25, 30},
    };

    std::vector<uint8_t> payload = encode_tick_payload(ticks);

    uint32_t first = impl.function(payload.data(), payload.size());
    uint32_t second = impl.function(payload.data(), payload.size());

    expect_eq(
        stats,
        impl.name,
        "encoded tick payload is deterministic",
        first,
        second
    );
}

static void test_tick_payload_corruption(
    const crcImpl& impl,
    testStats& stats
) {
    std::vector<Tick> ticks = {
        {1234567890ULL, 1.23, 10},
        {1234567990ULL, 1.24, 20},
        {1234568090ULL, 1.25, 30},
    };

    std::vector<uint8_t> original = encode_tick_payload(ticks);
    std::vector<uint8_t> corrupted = original; // copy constructor

    // flip one bit in encoded WAL payload, simulates disk corruption or bad write
    corrupted[5] ^= 0x01u;

    uint32_t crc_original = impl.function(original.data(), original.size());
    uint32_t crc_corrupted = impl.function(corrupted.data(), corrupted.size());

    expect_ne(
        stats,
        impl.name,
        "encoded tick payload corruption changes CRC",
        crc_original,
        crc_corrupted
    );
}

static void test_tick_payload_truncation(
    const crcImpl& impl,
    testStats& stats
) {
    std::vector<Tick> ticks = {
        {1234567890ULL, 1.23, 10},
        {1234567990ULL, 1.24, 20},
        {1234568090ULL, 1.25, 30},
    };

    std::vector<uint8_t> original = encode_tick_payload(ticks);
    std::vector<uint8_t> truncated = original;

    // remove final byte, simulates partial WAL write
    truncated.pop_back();

    uint32_t crc_original = impl.function(original.data(), original.size());
    uint32_t crc_truncated = impl.function(truncated.data(), truncated.size());

    expect_ne(
        stats,
        impl.name,
        "encoded tick payload truncation changes CRC",
        crc_original,
        crc_truncated
    );
}

#ifdef HAS_ZLIB
static void test_byte_pattern_against_zlib(
    const crcImpl& impl,
    testStats& stats
) {
    std::vector<uint8_t> data;

    for (int i = 0; i < 4096; i++) {
        data.push_back(static_cast<uint8_t>(i & 0xFF));
    }

    uint32_t expected = compute_checksum_zlib_reference(data.data(), data.size());
    uint32_t actual = impl.function(data.data(), data.size());

    expect_eq(
        stats,
        impl.name,
        "matches zlib on byte pattern",
        expected,
        actual
    );
}

static void test_tick_payload_against_zlib(
    const crcImpl& impl,
    testStats& stats
) {
    std::vector<Tick> ticks = {
        {1710000000000000000ULL, 512.34, 100},
        {1710000000000000100ULL, 512.35, 50},
        {1710000000000000200ULL, 512.36, 75},
        {1710000000000000300ULL, 512.37, 125},
    };

    std::vector<uint8_t> payload = encode_tick_payload(ticks);

    uint32_t expected = compute_checksum_zlib_reference(payload.data(), payload.size());
    uint32_t actual = impl.function(payload.data(), payload.size());

    expect_eq(
        stats,
        impl.name,
        "encoded tick payload matches zlib",
        expected,
        actual
    );
}
#endif

void run_crc32_correctness_tests(
    const crcImpl& impl,
    testStats& stats
) {
    test_empty_input(impl, stats);
    test_standard_string(impl, stats);
    test_changed_byte(impl, stats);
    test_truncated_input(impl, stats);
    test_same_input_twice(impl, stats);

    // TickDB-specific WAL payload tests.
    test_tick_payload_same_input_twice(impl, stats);
    test_tick_payload_corruption(impl, stats);
    test_tick_payload_truncation(impl, stats);

#ifdef HAS_ZLIB
    // extra confidence checks against industry-standard zlib
    test_byte_pattern_against_zlib(impl, stats);
    test_tick_payload_against_zlib(impl, stats);
#endif
}