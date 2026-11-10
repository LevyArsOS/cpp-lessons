#include "MemoryPool/include/projectile.hpp"
#include "MemoryPool/include/naive_spawner.hpp"
#include "MemoryPool/include/free_list_pool.hpp"
#include "MemoryPool/include/arena_allocator.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

namespace {

void test_projectile_lifecycle() {
    memory_pool::Projectile p;
    p.reset(10.0f, 20.0f, 1.0f, 2.0f, 2);
    assert(p.active);
    assert(p.x == 10.0f);
    assert(p.y == 20.0f);

    p.update(1.0f);
    assert(p.active);
    assert(p.x == 11.0f);
    assert(p.y == 22.0f);
    assert(p.ttl == 1);

    p.update(1.0f);
    assert(!p.active);
    assert(p.ttl == 0);
}

void test_free_list_pool_recycling_and_exhaustion() {
    constexpr std::size_t capacity = 4;
    memory_pool::FreeListPool<memory_pool::Projectile, capacity> pool;

    assert(pool.available() == 4);

    auto* p1 = pool.allocate();
    auto* p2 = pool.allocate();
    auto* p3 = pool.allocate();
    auto* p4 = pool.allocate();

    assert(p1 && p2 && p3 && p4);
    assert(pool.available() == 0);

    // Capacity exhaustion
    auto* p5 = pool.allocate();
    assert(p5 == nullptr);

    // Recycle slot
    pool.deallocate(p3);
    assert(pool.available() == 1);

    auto* recycled = pool.allocate();
    assert(recycled == p3);
    assert(pool.available() == 0);

    // Nullptr deallocation safe
    pool.deallocate(nullptr);

    pool.deallocate(p1);
    pool.deallocate(p2);
    pool.deallocate(recycled);
    pool.deallocate(p4);
    assert(pool.available() == 4);
}

void test_arena_allocator_bump_and_reset() {
    constexpr std::size_t bytes = 256;
    memory_pool::ArenaAllocator<bytes> arena;

    assert(arena.used_bytes() == 0);

    auto* p1 = arena.create<memory_pool::Projectile>();
    assert(p1 != nullptr);
    assert(arena.used_bytes() >= sizeof(memory_pool::Projectile));

    auto* p2 = arena.create<memory_pool::Projectile>();
    assert(p2 != nullptr);
    assert(p2 != p1);

    arena.reset();
    assert(arena.used_bytes() == 0);

    auto* p_new = arena.create<memory_pool::Projectile>();
    assert(p_new == p1); // reuses the buffer beginning

    // Exhaustion test
    void* huge = arena.allocate(1000);
    assert(huge == nullptr);
}

void test_spawners_equivalence() {
    constexpr std::size_t count = 100;
    constexpr float dt = 0.016f;

    memory_pool::NaiveSpawner naive;
    memory_pool::PoolSpawner<count> pool_spawner;
    memory_pool::ArenaSpawner<count * sizeof(memory_pool::Projectile) * 2> arena_spawner;

    for (std::size_t i = 0; i < count; ++i) {
        float vx = static_cast<float>(i % 5) + 0.5f;
        float vy = static_cast<float>(i % 3) + 1.0f;
        naive.spawn(0.0f, 0.0f, vx, vy, 10);
        pool_spawner.spawn(0.0f, 0.0f, vx, vy, 10);
        arena_spawner.spawn(0.0f, 0.0f, vx, vy, 10);
    }

    assert(naive.active_count() == count);
    assert(pool_spawner.active_count() == count);
    assert(arena_spawner.active_count() == count);

    for (int step = 0; step < 5; ++step) {
        naive.update(dt);
        pool_spawner.update(dt);
        arena_spawner.update(dt);
    }

    // Still active
    assert(naive.active_count() == count);
    assert(pool_spawner.active_count() == count);

    // Run until TTL expiry
    for (int step = 0; step < 6; ++step) {
        naive.update(dt);
        pool_spawner.update(dt);
    }

    assert(naive.active_count() == 0);
    assert(pool_spawner.active_count() == 0);
    assert(pool_spawner.pool_available() == count);
}

} // namespace

int main() {
    test_projectile_lifecycle();
    test_free_list_pool_recycling_and_exhaustion();
    test_arena_allocator_bump_and_reset();
    test_spawners_equivalence();
    std::cout << "All MemoryPool tests passed successfully.\n";
    return 0;
}
