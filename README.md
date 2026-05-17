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
- Slicing by 4 Table-based CRC32 with a 4x256 compile-time generated lookup table
- Slicing by 8 Table-based CRC32 with a 8x256 compile-time generated lookup table
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
| naive | 1.544475 sec | 64.75 MiB/s |
| table | 0.280905 sec | 355.99 MiB/s |
| 4-slice | 0.087268 sec | 1145.89 MiB/s |
| 8-slice | 0.154004 sec | 1340.07 MiB/s |
| zlib | 0.065098 sec | 1536.16 MiB/s |

### Encoded TickDB Payload, 100k Ticks, 100 Iterations

| Implementation | Time | Throughput |
|---|---:|---:|
| naive | 2.807496 sec | 67.94 MiB/s |
| table | 0.545248 sec | 349.81 MiB/s |
| 4-slice | 0.139663 sec | 1115.81 MiB/s |
| 8-slice | 0.154004 sec | 1365.68 MiB/s |
| zlib | 0.110182 sec | 1731.10 MiB/s |

## Benchmark Notes

The table-based version is much faster than the naive version, which is expected because it replaces the 8-round bit loop with a lookup table. The 4-slice is another big leap in performance, which is expected due to being able to calculate 4 bytes at once. However, while the 8-slice did see an improvement, it was marginal, which may be due to the fact that our data size is not large enough to see a bigger improvment.

On the both the random byte benchmark and the TickDB payload benchark, the 8-slice version is about 20x faster than the naive version.

`zlib` is still marginally faster than the 8-slice implementations, which I predict wil have a lrager and larger gap as payloads increase. 

## Work Done

| Implementation | Status | Notes |
|---|---|---|
| Naive CRC32 | Complete | Correct but slow |
| Table CRC32 | Complete | Correct and faster than naive |
| 4-slice CRC32 | Complete | Correct and faster than table |
| 8-slice CRC32 | Complete | Correct and faster than 4-slice |
| zlib CRC32 | Tested | Current reference implementation |

## Results

As it can be seen from the Benchmark tests, `zlib` is able to stay consistently faster than my implementations of the crc32 checksum. For this reason, the final version of TickDB will likely use the `zlib` library.
