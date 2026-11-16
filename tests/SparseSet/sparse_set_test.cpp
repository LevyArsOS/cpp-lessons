#include "SparseSet/include/sparse_set.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace cpp_lessons::sparse_set;

namespace {

template <typename Storage>
void verify_invariants_and_parity(const SparseSet<Velocity>& sparse, const Storage& reference, EntityId max_id) {
    assert(sparse.size() == reference.size());

    const auto entities = sparse.dense_entities();
    const auto data = sparse.dense_data();
    assert(entities.size() == sparse.size());
    assert(data.size() == sparse.size());

    for (size_t i = 0; i < entities.size(); ++i) {
        const EntityId id = entities[i];
        assert(sparse.contains(id));
        const Velocity* s_val = sparse.get(id);
        const Velocity* r_val = reference.get(id);
        assert(s_val != nullptr && r_val != nullptr);
        assert(s_val == &data[i]);
        assert(std::abs(s_val->vx - r_val->vx) < 1e-6f);
        assert(std::abs(s_val->vy - r_val->vy) < 1e-6f);
        assert(std::abs(s_val->vz - r_val->vz) < 1e-6f);
        assert(std::abs(s_val->drag - r_val->drag) < 1e-6f);
    }

    for (EntityId id = 0; id <= max_id; ++id) {
        assert(sparse.contains(id) == reference.contains(id));
    }
}

void test_basic_operations_and_swap_and_pop() {
    SparseSet<Velocity> sparse;
    UnorderedMapStorage<Velocity> map;
    FlatOpenMapStorage<Velocity> flat;

    sparse.reserve(16, 20);
    map.reserve(16);
    flat.reserve(16);

    const std::vector<EntityId> ids = {2, 7, 11, 15, 19};
    for (EntityId id : ids) {
        const Velocity v{static_cast<float>(id), static_cast<float>(id) * 2.0f, 1.5f, 0.98f};
        sparse.insert(id, v);
        map.insert(id, v);
        flat.insert(id, v);
    }

    // Overwrite existing entry.
    const Velocity updated{42.0f, 84.0f, 3.0f, 0.95f};
    sparse.insert(11, updated);
    map.insert(11, updated);
    flat.insert(11, updated);

    verify_invariants_and_parity(sparse, map, 25);
    verify_invariants_and_parity(sparse, flat, 25);

    // Remove first, middle, and last dense entries.
    assert(sparse.remove(2));
    assert(map.remove(2));
    assert(flat.remove(2));
    verify_invariants_and_parity(sparse, map, 25);

    assert(sparse.remove(19));
    assert(map.remove(19));
    assert(flat.remove(19));
    verify_invariants_and_parity(sparse, map, 25);

    assert(sparse.remove(11));
    assert(map.remove(11));
    assert(flat.remove(11));
    verify_invariants_and_parity(sparse, map, 25);

    // Re-insert previously removed entity ID.
    const Velocity reinserted{9.0f, 18.0f, 27.0f, 0.99f};
    sparse.insert(2, reinserted);
    map.insert(2, reinserted);
    flat.insert(2, reinserted);
    verify_invariants_and_parity(sparse, map, 25);
    verify_invariants_and_parity(sparse, flat, 25);
}

void test_edge_cases_and_fault_injection() {
    SparseSet<Velocity> sparse;
    FlatOpenMapStorage<Velocity> flat;

    // Out-of-range lookup and removal on empty container.
    assert(!sparse.contains(999999));
    assert(sparse.get(999999) == nullptr);
    assert(!sparse.remove(999999));
    assert(!flat.remove(999999));

    // Insert single element, double remove, and verify empty state.
    sparse.insert(5, Velocity{1.0f, 2.0f, 3.0f, 0.5f});
    flat.insert(5, Velocity{1.0f, 2.0f, 3.0f, 0.5f});
    assert(sparse.remove(5));
    assert(!sparse.remove(5));
    assert(flat.remove(5));
    assert(!flat.remove(5));
    assert(sparse.size() == 0);
    assert(flat.size() == 0);
}

void test_large_scale_parity() {
    constexpr EntityId kEntitySpace = 100'000;
    SparseSet<Velocity> sparse;
    UnorderedMapStorage<Velocity> map;
    FlatOpenMapStorage<Velocity> flat;

    sparse.reserve(kEntitySpace, kEntitySpace);
    map.reserve(kEntitySpace);
    flat.reserve(kEntitySpace);

    for (EntityId id = 0; id < kEntitySpace; ++id) {
        const Velocity v{
            static_cast<float>(id % 100),
            static_cast<float>((id * 3) % 100),
            static_cast<float>((id * 7) % 100),
            0.995f
        };
        sparse.insert(id, v);
        map.insert(id, v);
        flat.insert(id, v);
    }

    // Remove 50,000 entities deterministically.
    for (EntityId id = 0; id < kEntitySpace; id += 2) {
        assert(sparse.remove(id));
        assert(map.remove(id));
        assert(flat.remove(id));
    }

    assert(sparse.size() == 50'000);
    assert(map.size() == 50'000);
    assert(flat.size() == 50'000);

    // Dense iteration update parity.
    for (Velocity& v : sparse.dense_data()) {
        v.vx *= v.drag;
        v.vy *= v.drag;
    }
    map.for_each([](EntityId, Velocity& v) {
        v.vx *= v.drag;
        v.vy *= v.drag;
    });
    flat.for_each([](EntityId, Velocity& v) {
        v.vx *= v.drag;
        v.vy *= v.drag;
    });

    double sparse_sum = 0.0;
    for (const Velocity& v : sparse.dense_data()) {
        sparse_sum += static_cast<double>(v.vx) + static_cast<double>(v.vy);
    }
    double map_sum = 0.0;
    map.for_each([&](EntityId, const Velocity& v) {
        map_sum += static_cast<double>(v.vx) + static_cast<double>(v.vy);
    });
    double flat_sum = 0.0;
    flat.for_each([&](EntityId, const Velocity& v) {
        flat_sum += static_cast<double>(v.vx) + static_cast<double>(v.vy);
    });

    assert(std::abs(sparse_sum - map_sum) < 1e-2);
    assert(std::abs(sparse_sum - flat_sum) < 1e-2);
}

}  // namespace

int main() {
    test_basic_operations_and_swap_and_pop();
    test_edge_cases_and_fault_injection();
    test_large_scale_parity();
    std::cout << "All SparseSet tests passed.\n";
    return 0;
}
