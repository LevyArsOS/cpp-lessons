#include "CustomAllocator/include/pmr_frame_scratch.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory_resource>
#include <new>

using namespace cpp_lessons::custom_allocator;

namespace {

void test_parity_and_zero_upstream_monotonic() {
    CountingResource counter(std::pmr::null_memory_resource());
    HeapFrameBuilder heap_builder;
    MonotonicFrameBuilder monotonic_builder(&counter);

    constexpr uint32_t kBuckets = 64;
    constexpr uint32_t kItems = 16;

    uint64_t heap_sum = 0;
    uint64_t mono_sum = 0;
    for (int frame = 0; frame < 500; ++frame) {
        heap_sum += heap_builder.run_frame(kBuckets, kItems);
        mono_sum += monotonic_builder.run_frame(kBuckets, kItems);
    }

    assert(heap_sum == mono_sum);
    assert(counter.allocations() == 0);
    assert(counter.bytes_allocated() == 0);
}

void test_pool_recycles_intra_frame_blocks() {
    CountingResource counter;
    HeapFrameBuilder heap_builder;
    PoolFrameBuilder pool_builder(&counter);

    constexpr uint32_t kBuckets = 64;
    constexpr uint32_t kItems = 16;

    const uint64_t expected = heap_builder.run_frame(kBuckets, kItems);
    const uint64_t first_frame = pool_builder.run_frame(kBuckets, kItems);
    assert(first_frame == expected);

    // Steady-state frames should recycle pooled blocks without requesting new upstream chunks.
    counter.reset_counters();
    for (int frame = 0; frame < 100; ++frame) {
        assert(pool_builder.run_frame(kBuckets, kItems) == expected);
    }
    assert(counter.allocations() == 0);
}

void test_overflow_fallback_and_null_resource_throws() {
    CountingResource fallback_counter;
    MonotonicFrameBuilder fallback_builder(&fallback_counter);

    // Force growth beyond the 64 KB stack buffer (10,000 DrawCommands > 160 KB).
    const uint64_t sum = fallback_builder.run_frame(1, 10'000);
    assert(sum > 0);
    assert(fallback_counter.allocations() > 0);

    // Chaining to null_memory_resource must throw std::bad_alloc on buffer exhaustion.
    MonotonicFrameBuilder strict_builder(std::pmr::null_memory_resource());
    bool threw_bad_alloc = false;
    try {
        (void)strict_builder.run_frame(1, 10'000);
    } catch (const std::bad_alloc&) {
        threw_bad_alloc = true;
    }
    assert(threw_bad_alloc);
}

}  // namespace

int main() {
    test_parity_and_zero_upstream_monotonic();
    test_pool_recycles_intra_frame_blocks();
    test_overflow_fallback_and_null_resource_throws();
    std::cout << "All CustomAllocator tests passed.\n";
    return 0;
}
