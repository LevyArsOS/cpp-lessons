#include "SparseSet/include/sparse_set.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

using namespace cpp_lessons::sparse_set;

namespace {

constexpr EntityId kEntityCount = 100'000;
constexpr int kIterationFrames = 100;
constexpr size_t kEraseCount = 50'000;

struct BenchResult {
    double insert_ms{};
    double lookup_ms{};
    double iterate_ms{};
    double erase_ms{};
    double total_ms{};
    double checksum{};
};

template <typename Storage>
BenchResult run_benchmark(const std::vector<EntityId>& lookup_order, const std::vector<EntityId>& erase_order) {
    Storage storage;
    storage.reserve(kEntityCount);

    const auto t0 = std::chrono::steady_clock::now();
    for (EntityId id = 0; id < kEntityCount; ++id) {
        storage.insert(id, Velocity{
            static_cast<float>((id % 64) + 1),
            static_cast<float>((id % 32) + 1),
            static_cast<float>((id % 16) + 1),
            0.999f
        });
    }
    const auto t1 = std::chrono::steady_clock::now();

    double sum = 0.0;
    for (EntityId id : lookup_order) {
        if (const Velocity* v = storage.get(id)) {
            sum += v->vx + v->vy;
        }
    }
    const auto t2 = std::chrono::steady_clock::now();

    for (int frame = 0; frame < kIterationFrames; ++frame) {
        storage.for_each([](EntityId, Velocity& v) {
            v.vx *= v.drag;
            v.vy *= v.drag;
            v.vz *= v.drag;
        });
    }
    storage.for_each([&](EntityId, const Velocity& v) {
        sum += v.vx + v.vy + v.vz;
    });
    const auto t3 = std::chrono::steady_clock::now();

    for (size_t i = 0; i < kEraseCount; ++i) {
        storage.remove(erase_order[i]);
    }
    const auto t4 = std::chrono::steady_clock::now();

    auto to_ms = [](auto start, auto end) {
        return std::chrono::duration<double, std::milli>(end - start).count();
    };

    BenchResult res;
    res.insert_ms = to_ms(t0, t1);
    res.lookup_ms = to_ms(t1, t2);
    res.iterate_ms = to_ms(t2, t3);
    res.erase_ms = to_ms(t3, t4);
    res.total_ms = to_ms(t0, t4);
    res.checksum = sum;
    return res;
}

template <>
BenchResult run_benchmark<SparseSet<Velocity>>(
    const std::vector<EntityId>& lookup_order,
    const std::vector<EntityId>& erase_order) {
    SparseSet<Velocity> storage;
    storage.reserve(kEntityCount, kEntityCount);

    const auto t0 = std::chrono::steady_clock::now();
    for (EntityId id = 0; id < kEntityCount; ++id) {
        storage.insert(id, Velocity{
            static_cast<float>((id % 64) + 1),
            static_cast<float>((id % 32) + 1),
            static_cast<float>((id % 16) + 1),
            0.999f
        });
    }
    const auto t1 = std::chrono::steady_clock::now();

    double sum = 0.0;
    for (EntityId id : lookup_order) {
        if (const Velocity* v = storage.get(id)) {
            sum += v->vx + v->vy;
        }
    }
    const auto t2 = std::chrono::steady_clock::now();

    auto dense = storage.dense_data();
    for (int frame = 0; frame < kIterationFrames; ++frame) {
        for (Velocity& v : dense) {
            v.vx *= v.drag;
            v.vy *= v.drag;
            v.vz *= v.drag;
        }
    }
    for (const Velocity& v : dense) {
        sum += v.vx + v.vy + v.vz;
    }
    const auto t3 = std::chrono::steady_clock::now();

    for (size_t i = 0; i < kEraseCount; ++i) {
        storage.remove(erase_order[i]);
    }
    const auto t4 = std::chrono::steady_clock::now();

    auto to_ms = [](auto start, auto end) {
        return std::chrono::duration<double, std::milli>(end - start).count();
    };

    BenchResult res;
    res.insert_ms = to_ms(t0, t1);
    res.lookup_ms = to_ms(t1, t2);
    res.iterate_ms = to_ms(t2, t3);
    res.erase_ms = to_ms(t3, t4);
    res.total_ms = to_ms(t0, t4);
    res.checksum = sum;
    return res;
}

void print_row(const char* name, const BenchResult& r, const BenchResult& baseline) {
    std::cout << std::left << std::setw(22) << name
              << std::right << std::fixed << std::setprecision(3)
              << std::setw(10) << r.insert_ms
              << std::setw(10) << r.lookup_ms
              << std::setw(11) << r.iterate_ms
              << std::setw(10) << r.erase_ms
              << std::setw(10) << r.total_ms
              << std::setw(9) << std::setprecision(2) << (baseline.total_ms / r.total_ms) << "x"
              << std::setw(14) << std::setprecision(1) << r.checksum << "\n";
}

}  // namespace

int main() {
    std::vector<EntityId> lookup_order(kEntityCount);
    std::iota(lookup_order.begin(), lookup_order.end(), 0);
    std::mt19937 rng(42);
    std::shuffle(lookup_order.begin(), lookup_order.end(), rng);

    std::vector<EntityId> erase_order = lookup_order;
    std::shuffle(erase_order.begin(), erase_order.end(), rng);

    const BenchResult map_res = run_benchmark<UnorderedMapStorage<Velocity>>(lookup_order, erase_order);
    const BenchResult flat_res = run_benchmark<FlatOpenMapStorage<Velocity>>(lookup_order, erase_order);
    const BenchResult sparse_res = run_benchmark<SparseSet<Velocity>>(lookup_order, erase_order);

    std::cout << "SparseSet Benchmark (100,000 entities, 100 iteration frames, 50,000 erasures)\n\n";
    std::cout << std::left << std::setw(22) << "Container"
              << std::right
              << std::setw(10) << "Insert(ms)"
              << std::setw(10) << "Lookup(ms)"
              << std::setw(11) << "Iter(ms)"
              << std::setw(10) << "Erase(ms)"
              << std::setw(10) << "Total(ms)"
              << std::setw(10) << "Speedup"
              << std::setw(14) << "Checksum\n";
    std::cout << std::string(97, '-') << "\n";

    print_row("std::unordered_map", map_res, map_res);
    print_row("FlatOpenMap", flat_res, map_res);
    print_row("SparseSet<T>", sparse_res, map_res);
    return 0;
}
