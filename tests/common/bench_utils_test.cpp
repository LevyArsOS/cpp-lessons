#include "common/bench_utils.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

int main() {
    // 1. do_not_optimize accepts scalars, structs, and heap-backed objects
    int scalar = 42;
    bench::do_not_optimize(scalar);

    struct Pair {
        std::uint64_t a;
        double b;
    } pair{123, 4.5};
    bench::do_not_optimize(pair);

    std::string text = "benchmark";
    bench::do_not_optimize(text);
    bench::clobber_memory();

    // 2. measure_ms executes the callable and returns non-negative elapsed ms
    int calls = 0;
    const double elapsed = bench::measure_ms([&]() {
        std::uint64_t acc = 0;
        for (int i = 0; i < 1000; ++i) {
            acc += static_cast<std::uint64_t>(i);
        }
        bench::do_not_optimize(acc);
        ++calls;
    });
    assert(calls == 1);
    assert(elapsed >= 0.0);

    std::cout << "All bench_utils tests passed successfully.\n";
    return 0;
}
