#include "MemoryPool/include/projectile.hpp"
#include "MemoryPool/include/naive_spawner.hpp"
#include "MemoryPool/include/free_list_pool.hpp"
#include "MemoryPool/include/arena_allocator.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

int main() {
    constexpr std::size_t operations = 50000;
    constexpr std::size_t batch_size = 500;
    constexpr std::size_t batches = operations / batch_size;

    std::cout << "Running MemoryPool benchmark (" << operations << " operations)...\n";

    // 1. Naive Heap Allocation (new/delete)
    auto start_naive = std::chrono::high_resolution_clock::now();
    {
        memory_pool::NaiveSpawner naive;
        for (std::size_t b = 0; b < batches; ++b) {
            for (std::size_t i = 0; i < batch_size; ++i) {
                naive.spawn(static_cast<float>(i), 0.0f, 1.0f, 2.0f, 3);
            }
            for (int step = 0; step < 4; ++step) {
                naive.update(0.016f);
            }
        }
    }
    auto end_naive = std::chrono::high_resolution_clock::now();
    double naive_ns = std::chrono::duration<double, std::nano>(end_naive - start_naive).count();

    // 2. Free-List Pool
    auto start_pool = std::chrono::high_resolution_clock::now();
    {
        memory_pool::PoolSpawner<batch_size> pool_spawner;
        for (std::size_t b = 0; b < batches; ++b) {
            for (std::size_t i = 0; i < batch_size; ++i) {
                pool_spawner.spawn(static_cast<float>(i), 0.0f, 1.0f, 2.0f, 3);
            }
            for (int step = 0; step < 4; ++step) {
                pool_spawner.update(0.016f);
            }
        }
    }
    auto end_pool = std::chrono::high_resolution_clock::now();
    double pool_ns = std::chrono::duration<double, std::nano>(end_pool - start_pool).count();

    // 3. Linear Arena Allocator
    auto start_arena = std::chrono::high_resolution_clock::now();
    {
        memory_pool::ArenaSpawner<batch_size * sizeof(memory_pool::Projectile) * 2> arena_spawner;
        for (std::size_t b = 0; b < batches; ++b) {
            for (std::size_t i = 0; i < batch_size; ++i) {
                arena_spawner.spawn(static_cast<float>(i), 0.0f, 1.0f, 2.0f, 3);
            }
            for (int step = 0; step < 4; ++step) {
                arena_spawner.update(0.016f);
            }
            arena_spawner.reset_burst();
        }
    }
    auto end_arena = std::chrono::high_resolution_clock::now();
    double arena_ns = std::chrono::duration<double, std::nano>(end_arena - start_arena).count();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nResults:\n";
    std::cout << "Naive new/delete:  " << (naive_ns / 1e6) << " ms (" << (naive_ns / operations) << " ns/op) [Baseline 1.00x]\n";
    std::cout << "FreeListPool:      " << (pool_ns / 1e6) << " ms (" << (pool_ns / operations) << " ns/op) [" << (pool_ns / naive_ns) << "x]\n";
    std::cout << "ArenaAllocator:    " << (arena_ns / 1e6) << " ms (" << (arena_ns / operations) << " ns/op) [" << (arena_ns / naive_ns) << "x]\n";

    return 0;
}
