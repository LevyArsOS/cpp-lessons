#include "VectorVsList/include/entity_containers.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>

template <typename World>
static double run_workload(std::size_t initial_count, std::size_t removals, int frames) {
    World world;
    for (std::uint32_t i = 0; i < initial_count; ++i) {
        world.spawn(i, static_cast<float>(i % 100), 0.0f, 1.5f, -0.5f);
    }

    auto start = std::chrono::high_resolution_clock::now();
    volatile float sink = 0.0f;

    for (int f = 0; f < frames; ++f) {
        world.update_all(0.016f);
        for (std::size_t r = 0; r < removals && world.size() > 1; ++r) {
            std::size_t idx = (r * 7919 + static_cast<std::size_t>(f) * 101) % world.size();
            world.remove_at(idx);
            world.spawn(static_cast<std::uint32_t>(initial_count + r), 0.0f, 0.0f, 1.5f, -0.5f);
        }
    }

    for (const auto& e : world.items()) {
        sink += e.x + e.y;
    }

    auto end = std::chrono::high_resolution_clock::now();
    (void)sink;
    return std::chrono::duration<double, std::milli>(end - start).count();
}

int main() {
    constexpr std::size_t counts[] = {10000, 100000};
    constexpr std::size_t removals_per_frame = 200;
    constexpr int frames = 20;

    std::cout << std::fixed << std::setprecision(3);
    for (std::size_t n : counts) {
        double list_ms = run_workload<vector_vs_list::ListWorld>(n, removals_per_frame, frames);
        double deque_ms = run_workload<vector_vs_list::DequeWorld>(n, removals_per_frame, frames);
        double vec_ms = run_workload<vector_vs_list::VectorSwapPopWorld>(n, removals_per_frame, frames);

        std::cout << "=== N = " << n << " (" << frames << " frames, " << removals_per_frame << " removals/frame) ===\n";
        std::cout << "std::list:              " << list_ms << " ms\n";
        std::cout << "std::deque:             " << deque_ms << " ms\n";
        std::cout << "std::vector (swap-pop): " << vec_ms << " ms (" << (list_ms / vec_ms) << "x faster vs list)\n\n";
    }
    return 0;
}
