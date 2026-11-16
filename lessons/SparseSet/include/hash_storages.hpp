#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

namespace cpp_lessons::sparse_set {

using EntityId = uint32_t;
inline constexpr uint32_t kInvalidIndex = std::numeric_limits<uint32_t>::max();

struct Velocity {
    float vx{};
    float vy{};
    float vz{};
    float drag{};
};

/// Baseline: node-based std::unordered_map<EntityId, T>.
/// Allocates a heap node per entry and scatters elements across memory.
template <typename T>
class UnorderedMapStorage {
public:
    void reserve(size_t count) { map_.reserve(count); }
    void insert(EntityId id, const T& value) { map_.insert_or_assign(id, value); }
    [[nodiscard]] bool contains(EntityId id) const { return map_.find(id) != map_.end(); }

    [[nodiscard]] T* get(EntityId id) {
        auto it = map_.find(id);
        return it != map_.end() ? &it->second : nullptr;
    }

    [[nodiscard]] const T* get(EntityId id) const {
        auto it = map_.find(id);
        return it != map_.end() ? &it->second : nullptr;
    }

    bool remove(EntityId id) { return map_.erase(id) > 0; }

    template <typename Fn>
    void for_each(Fn&& fn) {
        for (auto& [id, value] : map_) {
            fn(id, value);
        }
    }

    [[nodiscard]] size_t size() const { return map_.size(); }
    void clear() { map_.clear(); }

private:
    std::unordered_map<EntityId, T> map_;
};

/// Intermediate: open-addressing hash map with linear probing in a single contiguous vector.
/// Eliminates per-node heap allocations, though full iteration still scans empty/tombstone slots.
template <typename T>
class FlatOpenMapStorage {
public:
    explicit FlatOpenMapStorage(size_t initial_capacity = 16) {
        rehash(next_pow2(initial_capacity < 16 ? 16 : initial_capacity));
    }

    void reserve(size_t expected_count) {
        const size_t required = expected_count * 2;
        if (required > slots_.size()) {
            rehash(next_pow2(required));
        }
    }

    void insert(EntityId id, const T& value) {
        if ((size_ + tombstones_ + 1) * 10 >= slots_.size() * 7) {
            rehash(slots_.size() * 2);
        }

        const size_t mask = slots_.size() - 1;
        size_t idx = hash_id(id) & mask;
        size_t first_tombstone = kNoSlot;

        while (true) {
            Slot& slot = slots_[idx];
            if (slot.state == SlotState::Empty) {
                const size_t target = (first_tombstone != kNoSlot) ? first_tombstone : idx;
                if (slots_[target].state == SlotState::Tombstone) {
                    --tombstones_;
                }
                slots_[target] = Slot{id, value, SlotState::Occupied};
                ++size_;
                return;
            }
            if (slot.state == SlotState::Occupied && slot.key == id) {
                slot.value = value;
                return;
            }
            if (slot.state == SlotState::Tombstone && first_tombstone == kNoSlot) {
                first_tombstone = idx;
            }
            idx = (idx + 1) & mask;
        }
    }

    [[nodiscard]] bool contains(EntityId id) const { return find_slot(id) != kNoSlot; }
    [[nodiscard]] T* get(EntityId id) {
        const size_t idx = find_slot(id);
        return idx != kNoSlot ? &slots_[idx].value : nullptr;
    }
    [[nodiscard]] const T* get(EntityId id) const {
        const size_t idx = find_slot(id);
        return idx != kNoSlot ? &slots_[idx].value : nullptr;
    }

    bool remove(EntityId id) {
        const size_t idx = find_slot(id);
        if (idx == kNoSlot) {
            return false;
        }
        slots_[idx].state = SlotState::Tombstone;
        --size_;
        ++tombstones_;
        return true;
    }

    template <typename Fn>
    void for_each(Fn&& fn) {
        for (Slot& slot : slots_) {
            if (slot.state == SlotState::Occupied) {
                fn(slot.key, slot.value);
            }
        }
    }

    [[nodiscard]] size_t size() const { return size_; }

    void clear() {
        for (Slot& slot : slots_) {
            slot.state = SlotState::Empty;
        }
        size_ = 0;
        tombstones_ = 0;
    }

private:
    enum class SlotState : uint8_t { Empty = 0, Occupied = 1, Tombstone = 2 };
    struct Slot {
        EntityId key{};
        T value{};
        SlotState state{SlotState::Empty};
    };

    static constexpr size_t kNoSlot = std::numeric_limits<size_t>::max();

    static size_t next_pow2(size_t n) {
        size_t p = 1;
        while (p < n) { p <<= 1; }
        return p;
    }

    static size_t hash_id(EntityId id) {
        uint32_t x = id;
        x ^= x >> 16;
        x *= 0x7feb352du;
        x ^= x >> 15;
        x *= 0x846ca68bu;
        x ^= x >> 16;
        return static_cast<size_t>(x);
    }

    [[nodiscard]] size_t find_slot(EntityId id) const {
        const size_t mask = slots_.size() - 1;
        size_t idx = hash_id(id) & mask;
        while (true) {
            const Slot& slot = slots_[idx];
            if (slot.state == SlotState::Empty) { return kNoSlot; }
            if (slot.state == SlotState::Occupied && slot.key == id) { return idx; }
            idx = (idx + 1) & mask;
        }
    }

    void rehash(size_t new_capacity) {
        std::vector<Slot> old_slots = std::move(slots_);
        slots_.assign(new_capacity, Slot{});
        size_ = 0;
        tombstones_ = 0;
        for (const Slot& slot : old_slots) {
            if (slot.state == SlotState::Occupied) {
                insert(slot.key, slot.value);
            }
        }
    }

    std::vector<Slot> slots_;
    size_t size_{0};
    size_t tombstones_{0};
};

}  // namespace cpp_lessons::sparse_set
