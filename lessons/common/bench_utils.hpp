#pragma once

#include <atomic>
#include <chrono>
#include <utility>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace bench {

template <typename T>
inline void do_not_optimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("" : : "r,m"(value) : "memory");
#elif defined(_MSC_VER)
    const volatile void* volatile sink = &value;
    (void)sink;
    _ReadWriteBarrier();
#else
    const volatile void* volatile sink = &value;
    (void)sink;
    std::atomic_signal_fence(std::memory_order_acq_rel);
#endif
}

inline void clobber_memory() {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("" : : : "memory");
#elif defined(_MSC_VER)
    _ReadWriteBarrier();
#else
    std::atomic_signal_fence(std::memory_order_acq_rel);
#endif
}

template <typename Fn>
[[nodiscard]] inline double measure_ms(Fn&& fn) {
    const auto start = std::chrono::high_resolution_clock::now();
    std::forward<Fn>(fn)();
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

} // namespace bench
