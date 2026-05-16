# CRC32 Checksum for TickDB

This project is a small CRC32 checksum module built for my TickDB time-series database project. The goal is to understand how CRC32 works, test multiple implementations, and benchmark them before using a checksum in TickDB's write-ahead log (WAL).

This is mainly for educational purposes so I can get better at C++, binary data handling, testing, and performance benchmarking. The final TickDB implementation will likely use `zlib` for CRC32 unless my own implementation can match or beat its performance in benchmarks.

## Purpose

TickDB will use CRC32 in the WAL to detect corrupted or partially written records during crash recovery. The checksum will be computed over the encoded WAL payload bytes, not over raw C++ objects.

For TickDB, a tick is encoded as a stable 20-byte payload:

```text
bytes 0..7   timestamp
bytes 8..15  price
bytes 16..19 volume
```

This avoids relying on struct padding, compiler layout, or C++ object internals.

## CRC32 Variant

This project uses the standard reflected CRC32 variant commonly used by `zlib`, Ethernet, zip, and many online CRC32 calculators.

```text
Initial value: 0xFFFFFFFF
Polynomial:    0xEDB88320
Final XOR:     0xFFFFFFFF
Bit order:     Reflected / LSB-first
```

Standard validation vector:

```text
Input:  "123456789"
Output: 0xCBF43926
```

## Implementations

Several CRC32 implementations are being written and tested:

- Naive bit-by-bit CRC32
- Table-based CRC32 with a compile-time generated lookup table
- Faster optimized CRC32 versions, planned
- `zlib` CRC32 reference comparison

All implementations are run through the same correctness tests and benchmarks.

## File Structure

```text
crc32-checksum/
  checksum.h
  checksum.cpp

  test_common.h
  test_common.cpp

  tick_test_utils.h
  tick_test_utils.cpp

  test_crc32_correctness.h
  test_crc32_correctness.cpp

  benchmark_crc32.h
  benchmark_crc32.cpp

  main.cpp
  Makefile
  README.md
```

## Build and Run

Run without `zlib`:

```bash
make run
```

Run with `zlib` comparison:

```bash
make run-zlib
```

Clean build output:

```bash
make clean
```

## Correctness Tests

The test suite checks:

- Empty input returns `0x00000000`
- `"123456789"` returns `0xCBF43926`
- Changing one byte changes the CRC
- Truncating one byte changes the CRC
- Same input twice returns the same CRC
- Encoded TickDB payloads are deterministic
- Corrupted TickDB payloads are detected
- Truncated TickDB payloads are detected
- Results match `zlib`, when enabled

Latest result with `make run-zlib`:

```text
Tests run:    30
Tests failed: 0
Result: PASSED
```

## Current Benchmarks

Benchmarks were run against `zlib` using `make run-zlib`.

Compile command:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -DHAS_ZLIB main.cpp checksum.cpp test_common.cpp tick_test_utils.cpp test_crc32_correctness.cpp benchmark_crc32.cpp -lz -o crc32_tests
```

### Random Bytes, 1 MiB, 100 Iterations

| Implementation | Time | Throughput |
|---|---:|---:|
| naive | 3.586249 sec | 27.88 MiB/s |
| table | 0.921510 sec | 108.52 MiB/s |
| zlib | 0.254647 sec | 392.70 MiB/s |

### Encoded TickDB Payload, 100k Ticks, 100 Iterations

| Implementation | Time | Throughput |
|---|---:|---:|
| naive | 10.146156 sec | 18.80 MiB/s |
| table | 2.011472 sec | 94.82 MiB/s |
| zlib | 0.482699 sec | 395.14 MiB/s |

## Benchmark Notes

The table-based version is much faster than the naive version, which is expected because it replaces the 8-round bit loop with a lookup table.

On the random byte benchmark, the table version is about 3.9x faster than the naive version.

On the encoded TickDB payload benchmark, the table version is about 5.0x faster than the naive version.

`zlib` is still significantly faster than both custom implementations, so it is currently the likely choice for the final TickDB WAL checksum unless a later optimized implementation gets closer.

## Future Work

| Implementation | Status | Notes |
|---|---|---|
| Naive CRC32 | Complete | Correct but slow |
| Table CRC32 | Complete | Correct and faster than naive |
| Optimized CRC32 | Planned | Could use slicing-by-4, slicing-by-8, or hardware support |
| zlib CRC32 | Tested | Current reference implementation |

## Next Steps

1. Clean up the checksum API.
2. Optionally try a slicing-by-4 or slicing-by-8 implementation.
3. Compare optimized versions against `zlib`.
4. Decide whether TickDB should use my implementation or `zlib`.
5. Integrate CRC32 into the TickDB WAL.
