#include "benchmark_crc32.h"
#include "tick_test_utils.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>
#include <vector>

static std::vector<uint8_t> generate_random_bytes(size_t size) {
    std::vector<uint8_t> data(size);

    // fixed seed
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, 255);

    for (size_t i = 0; i < size; i++) {
        data[i] = static_cast<uint8_t>(dist(rng));
    }

    return data;
}

static std::vector<uint8_t> generate_tick_payload(size_t tick_count) {
    std::vector<Tick> ticks;
    ticks.reserve(tick_count);

    uint64_t base_time = 1710000000000000000ULL;

    for (size_t i = 0; i < tick_count; i++) {
        Tick tick;

        tick.time = base_time + static_cast<uint64_t>(i * 100);
        tick.price = 500.0 + static_cast<double>(i % 1000) * 0.01;
        tick.volume = static_cast<uint32_t>((i % 100) + 1);

        ticks.push_back(tick);
    }

    return encode_tick_payload(ticks);
}

static double benchmark_one_impl(
    const crcImpl& impl,
    const std::vector<uint8_t>& data,
    int iterations,
    uint32_t* final_crc_out
) {
    using clock = std::chrono::high_resolution_clock;

    // prevents the compiler from optimizing away the checksum calls.
    volatile uint32_t sink = 0;

    auto start = clock::now();

    for (int i = 0; i < iterations; i++) {
        sink ^= impl.function(data.data(), data.size());
    }

    auto end = clock::now();

    *final_crc_out = sink;

    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

void run_crc32_benchmarks(const std::vector<crcImpl>& implementations) {
    std::cout << "\nCRC32 Benchmark Results\n";
    std::cout << "=======================\n";

    struct BenchmarkCase {
        const char* name;
        std::vector<uint8_t> data;
        int iterations;
    };

    std::vector<BenchmarkCase> cases;

    cases.push_back({
        "random bytes, 1 MiB",
        generate_random_bytes(1024 * 1024),
        100
    });

    cases.push_back({
        "encoded ticks, 100k ticks",
        generate_tick_payload(100000),
        100
    });

    for (const BenchmarkCase& bench_case : cases) {
        std::cout << "\nCase: " << bench_case.name << "\n";
        std::cout << "Input size: " << bench_case.data.size() << " bytes\n";
        std::cout << "Iterations: " << bench_case.iterations << "\n\n";

        for (const crcImpl& impl : implementations) {
            uint32_t final_crc = 0;

            double seconds = benchmark_one_impl(
                impl,
                bench_case.data,
                bench_case.iterations,
                &final_crc
            );

            double total_bytes =
                static_cast<double>(bench_case.data.size()) *
                static_cast<double>(bench_case.iterations);

            double mib_per_sec = total_bytes / seconds / (1024.0 * 1024.0);

            std::printf(
                "%-16s time: %.6f sec | throughput: %.2f MiB/s | sink: %08X\n",
                impl.name,
                seconds,
                mib_per_sec,
                final_crc
            );
        }
    }
}