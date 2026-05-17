#include "benchmark_crc32.h"
#include "checksum.h"
#include "test_common.h"
#include "test_crc32_correctness.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#ifdef HAS_ZLIB
#include <zlib.h>
static uint32_t compute_checksum_zlib(const uint8_t* data, size_t length) {
    uLong crc = crc32(0L, Z_NULL, 0);

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

int main() {

    // array of implementations being tested
    std::vector<crcImpl> implementations = {
        {"naive", compute_checksum_naive},
        {"table", compute_checksum_table},
        {"4-slice", compute_checksum_four_slice},
        {"8-slice", compute_checksum_eight_slice},
    };

#ifdef HAS_ZLIB
    implementations.push_back({"zlib", compute_checksum_zlib});
#endif

    testStats stats;

    std::cout << "CRC32 Correctness Tests\n";
    std::cout << "=======================\n";

    for (const crcImpl& impl : implementations) {
        run_crc32_correctness_tests(impl, stats);
    }

    print_test_summary(stats);

    if (stats.tests_failed != 0) {
        return 1;
    }

    run_crc32_benchmarks(implementations);

    return 0;
}