#include "test_common.h"

#include <cstdio>
#include <iostream>

static void print_pass(const char* impl_name, const char* test_name) {
    std::printf("[PASS] %-16s %s\n", impl_name, test_name);
}

static void print_fail(
    const char* impl_name,
    const char* test_name,
    uint32_t expected,
    uint32_t actual
) {
    std::printf(
        "[FAIL] %-16s %s | expected: %08X actual: %08X\n",
        impl_name,
        test_name,
        expected,
        actual
    );
}

void expect_eq(
    testStats& stats,
    const char* impl_name,
    const char* test_name,
    uint32_t expected,
    uint32_t actual
) {
    stats.tests_run++;

    if (expected == actual) {
        print_pass(impl_name, test_name);
    } else {
        stats.tests_failed++;
        print_fail(impl_name, test_name, expected, actual);
    }
}

void expect_ne(
    testStats& stats,
    const char* impl_name,
    const char* test_name,
    uint32_t left,
    uint32_t right
) {
    stats.tests_run++;

    if (left != right) {
        print_pass(impl_name, test_name);
    } else {
        stats.tests_failed++;

        std::printf(
            "[FAIL] %-16s %s | both values were: %08X\n",
            impl_name,
            test_name,
            left
        );
    }
}

void print_test_summary(const testStats& stats) {
    std::cout << "\nTest Summary\n";
    std::cout << "============\n";
    std::cout << "Tests run:    " << stats.tests_run << "\n";
    std::cout << "Tests failed: " << stats.tests_failed << "\n";

    if (stats.tests_failed == 0) {
        std::cout << "\nResult: PASSED\n";
    } else {
        std::cout << "\nResult: FAILED\n";
    }
}