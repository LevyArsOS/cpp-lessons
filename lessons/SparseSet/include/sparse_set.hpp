#pragma once

#include "SparseSet/include/hash_storages.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>

namespace cpp_lessons::sparse_set {

/// Optimized: ECS Sparse Set with O(1) unhashed lookup, O(1) swap-and-pop removal,
/// and tightly packed contiguous dense arrays for hardware-prefetched iteration.
template <typename T>
class SparseSet {
public:
    void reserve(size_t dense_capacity, EntityId max_entity_id) {
        dense_entities_.reserve(dense_capacity);
        dense_data_.reserve(dense_capacity);
        if (static_cast<size_t>(max_entity_id) + 1 > sparse_.size()) {
            sparse_.resize(static_cast<size_t>(max_entity_id) + 1, kInvalidIndex);
        }
    }

    void insert(EntityId id, const T& value) {
        if (contains(id)) {
            dense_data_[sparse_[id]] = value;
            return;
        }

        if (static_cast<size_t>(id) >= sparse_.size()) {
            sparse_.resize(static_cast<size_t>(id) + 1, kInvalidIndex);
        }

        const uint32_t dense_index = static_cast<uint32_t>(dense_entities_.size());
        sparse_[id] = dense_index;
        dense_entities_.push_back(id);
        dense_data_.push_back(value);
    }

    [[nodiscard]] bool contains(EntityId id) const {
        if (static_cast<size_t>(id) >= sparse_.size()) {
            return false;
        }
        const uint32_t dense_index = sparse_[id];
        return dense_index < dense_entities_.size() && dense_entities_[dense_index] == id;
    }

    [[nodiscard]] T* get(EntityId id) {
        return contains(id) ? &dense_data_[sparse_[id]] : nullptr;
    }

    [[nodiscard]] const T* get(EntityId id) const {
        return contains(id) ? &dense_data_[sparse_[id]] : nullptr;
    }

    bool remove(EntityId id) {
        if (!contains(id)) {
            return false;
        }

        const uint32_t removed_dense_idx = sparse_[id];
        const uint32_t last_dense_idx = static_cast<uint32_t>(dense_entities_.size() - 1);

        if (removed_dense_idx != last_dense_idx) {
            const EntityId moved_entity = dense_entities_[last_dense_idx];
            dense_entities_[removed_dense_idx] = moved_entity;
            dense_data_[removed_dense_idx] = std::move(dense_data_[last_dense_idx]);
            sparse_[moved_entity] = removed_dense_idx;
        }

        dense_entities_.pop_back();
        dense_data_.pop_back();
        sparse_[id] = kInvalidIndex;
        return true;
    }

    template <typename Fn>
    void for_each(Fn&& fn) {
        const size_t count = dense_data_.size();
        for (size_t i = 0; i < count; ++i) {
            fn(dense_entities_[i], dense_data_[i]);
        }
    }

    [[nodiscard]] std::span<T> dense_data() {
        return std::span<T>(dense_data_.data(), dense_data_.size());
    }

    [[nodiscard]] std::span<const T> dense_data() const {
        return std::span<const T>(dense_data_.data(), dense_data_.size());
    }

    [[nodiscard]] std::span<const EntityId> dense_entities() const {
        return std::span<const EntityId>(dense_entities_.data(), dense_entities_.size());
    }

    [[nodiscard]] size_t size() const {
        return dense_data_.size();
    }

    void clear() {
        for (EntityId id : dense_entities_) {
            sparse_[id] = kInvalidIndex;
        }
        dense_entities_.clear();
        dense_data_.clear();
    }

private:
    std::vector<uint32_t> sparse_;
    std::vector<EntityId> dense_entities_;
    std::vector<T> dense_data_;
};

}  // namespace cpp_lessons::sparse_set
