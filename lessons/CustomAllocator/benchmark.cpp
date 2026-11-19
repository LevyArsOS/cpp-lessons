#include "CustomAllocator/include/pmr_frame_scratch.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>

using namespace cpp_lessons::custom_allocator;

namespace {

constexpr int kFrames = 1'000;
constexpr uint32_t kBuckets = 64;
constexpr uint32_t kItemsPerBucket = 24;

template <typename Builder>
std::pair<double, uint64_t> measure(Builder& builder) {
    // Warmup
    volatile uint64_t sink = builder.run_frame(kBuckets, kItemsPerBucket);
    (void)sink;

    uint64_t checksum = 0;
    const auto t0 = std::chrono::steady_clock::now();
    for (int f = 0; f < kFrames; ++f) {
        checksum += builder.run_frame(kBuckets, kItemsPerBucket);
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return {ms, checksum};
}

}  // namespace

int main() {
    HeapFrameBuilder heap_builder;
    MonotonicFrameBuilder monotonic_builder;
    PoolFrameBuilder pool_builder;

    const auto [heap_ms, heap_sum] = measure(heap_builder);
    const auto [mono_ms, mono_sum] = measure(monotonic_builder);
    const auto [pool_ms, pool_sum] = measure(pool_builder);

    std::cout << "CustomAllocator Benchmark (" << kFrames << " frames x "
              << kBuckets << " dynamic vectors x " << kItemsPerBucket << " items)\n\n";
    std::cout << std::left << std::setw(34) << "Allocator"
              << std::right << std::setw(12) << "Time (ms)"
              << std::setw(10) << "Speedup"
              << std::setw(16) << "Checksum\n";
    std::cout << std::string(72, '-') << "\n";

    auto print_row = [&](const char* name, double ms, uint64_t sum) {
        std::cout << std::left << std::setw(34) << name
                  << std::right << std::fixed << std::setprecision(3)
                  << std::setw(12) << ms
                  << std::setw(9) << std::setprecision(2) << (heap_ms / ms) << "x"
                  << std::setw(16) << sum << "\n";
    };

    print_row("std::vector (default heap)", heap_ms, heap_sum);
    print_row("pmr::unsynchronized_pool", pool_ms, pool_sum);
    print_row("pmr::monotonic_buffer + release", mono_ms, mono_sum);
    return 0;
}
