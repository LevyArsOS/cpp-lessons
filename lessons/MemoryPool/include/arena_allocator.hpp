#pragma once

#include "projectile.hpp"
#include <cstddef>
#include <cstdint>
#include <new>
#include <utility>
#include <vector>

namespace memory_pool {

template <std::size_t Bytes>
class ArenaAllocator {
public:
    ArenaAllocator() = default;
    ~ArenaAllocator() = default;

    ArenaAllocator(const ArenaAllocator&) = delete;
    ArenaAllocator& operator=(const ArenaAllocator&) = delete;
    ArenaAllocator(ArenaAllocator&&) = delete;
    ArenaAllocator& operator=(ArenaAllocator&&) = delete;

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) noexcept {
        std::uintptr_t current = reinterpret_cast<std::uintptr_t>(buffer_ + offset_);
        std::uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
        std::size_t padding = aligned - current;

        if (offset_ + padding + size > Bytes) {
            return nullptr;
        }

        offset_ += padding + size;
        return reinterpret_cast<void*>(aligned);
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* mem = allocate(sizeof(T), alignof(T));
        if (!mem) return nullptr;
        return ::new (mem) T(std::forward<Args>(args)...);
    }

    void reset() noexcept {
        offset_ = 0;
    }

    [[nodiscard]] std::size_t used_bytes() const noexcept {
        return offset_;
    }

    [[nodiscard]] static constexpr std::size_t capacity_bytes() noexcept {
        return Bytes;
    }

private:
    alignas(std::max_align_t) std::byte buffer_[Bytes];
    std::size_t offset_{0};
};

template <std::size_t Bytes>
class ArenaSpawner {
public:
    Projectile* spawn(float x, float y, float vx, float vy, int ttl) {
        auto* p = arena_.template create<Projectile>();
        if (!p) return nullptr;
        p->reset(x, y, vx, vy, ttl);
        active_projectiles_.push_back(p);
        return p;
    }

    void update(float dt) {
        for (auto* p : active_projectiles_) {
            p->update(dt);
        }
    }

    void reset_burst() {
        active_projectiles_.clear();
        arena_.reset();
    }

    [[nodiscard]] std::size_t active_count() const noexcept {
        return active_projectiles_.size();
    }

    [[nodiscard]] std::size_t used_bytes() const noexcept {
        return arena_.used_bytes();
    }

private:
    ArenaAllocator<Bytes> arena_;
    std::vector<Projectile*> active_projectiles_;
};

} // namespace memory_pool
