#include "VectorVsList/include/entity_containers.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>

int main() {
    using vector_vs_list::DequeWorld;
    using vector_vs_list::ListWorld;
    using vector_vs_list::VectorSwapPopWorld;

    // 1. Swap-and-pop O(1) removal preserves remaining elements and contiguous storage
    VectorSwapPopWorld vec(8);
    for (std::uint32_t i = 0; i < 5; ++i) {
        vec.spawn(i, static_cast<float>(i), 0.0f, 1.0f, 2.0f);
    }
    assert(vec.size() == 5);

    // Contiguous memory check
    const auto* base = vec.data();
    for (std::size_t i = 0; i < vec.size(); ++i) {
        assert(&vec.items()[i] == base + i);
    }

    // Remove middle element (index 1 -> id 1 replaced by last id 4)
    assert(vec.remove_at(1));
    assert(vec.size() == 4);
    assert(vec.items()[1].id == 4);

    // Remove last element
    assert(vec.remove_at(3));
    assert(vec.size() == 3);

    // Out-of-bounds returns false safely
    assert(!vec.remove_at(99));

    // 2. Update parity across ListWorld, DequeWorld, and VectorSwapPopWorld
    ListWorld list_w;
    DequeWorld deque_w;
    VectorSwapPopWorld vec_w(10);

    for (std::uint32_t i = 0; i < 10; ++i) {
        list_w.spawn(i, 1.0f, 2.0f, 3.0f, -1.0f);
        deque_w.spawn(i, 1.0f, 2.0f, 3.0f, -1.0f);
        vec_w.spawn(i, 1.0f, 2.0f, 3.0f, -1.0f);
    }

    assert(list_w.remove_at(3));
    assert(deque_w.remove_at(3));
    assert(vec_w.remove_at(3));

    list_w.update_all(0.5f);
    deque_w.update_all(0.5f);
    vec_w.update_all(0.5f);

    assert(list_w.size() == 9);
    assert(deque_w.size() == 9);
    assert(vec_w.size() == 9);

    float sum_list = 0.0f;
    for (const auto& e : list_w.items()) sum_list += e.x + e.y;
    float sum_deque = 0.0f;
    for (const auto& e : deque_w.items()) sum_deque += e.x + e.y;
    float sum_vec = 0.0f;
    for (const auto& e : vec_w.items()) sum_vec += e.x + e.y;

    assert(std::fabs(sum_list - sum_vec) < 1e-4f);
    assert(std::fabs(sum_deque - sum_vec) < 1e-4f);

    std::cout << "All VectorVsList tests passed successfully.\n";
    return 0;
}
