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
- Table-based CRC32, planned
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
Tests run:    20
Tests failed: 0
Result: PASSED
```

## Current Benchmarks

Benchmarks were run against `zlib` using `make run-zlib`.

### Random Bytes, 1 MiB, 100 Iterations

| Implementation | Time | Throughput |
|---|---:|---:|
| naive | 1.770154 sec | 56.49 MiB/s |
| zlib | 0.083371 sec | 1199.46 MiB/s |

### Encoded TickDB Payload, 100k Ticks, 100 Iterations

| Implementation | Time | Throughput |
|---|---:|---:|
| naive | 3.114714 sec | 61.24 MiB/s |
| zlib | 0.151707 sec | 1257.26 MiB/s |

The naive version is correct but much slower than `zlib`, which is expected because it processes every byte one bit at a time.

## Future Benchmarks

| Implementation | Status | Notes |
|---|---|---|
| Naive CRC32 | Complete | Correct but slow |
| Table CRC32 | Planned | Should be much faster |
| Optimized CRC32 | Planned | Could use slicing-by-4, slicing-by-8, or hardware support |
| zlib CRC32 | Tested | Current reference implementation |

## Next Steps

1. Implement table-based CRC32.
2. Run the same correctness tests against it.
3. Benchmark naive vs table vs `zlib`.
4. Decide whether TickDB should use my implementation or `zlib`.
5. Integrate CRC32 into the TickDB WAL.
