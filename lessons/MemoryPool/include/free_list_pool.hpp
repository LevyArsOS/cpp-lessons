#pragma once

#include "projectile.hpp"
#include <cstddef>
#include <new>
#include <utility>
#include <vector>

namespace memory_pool {

template <typename T, std::size_t Capacity>
class FreeListPool {
public:
    FreeListPool() {
        for (std::size_t i = 0; i < Capacity - 1; ++i) {
            slots_[i].next = &slots_[i + 1];
        }
        slots_[Capacity - 1].next = nullptr;
        free_head_ = &slots_[0];
    }

    ~FreeListPool() = default;

    FreeListPool(const FreeListPool&) = delete;
    FreeListPool& operator=(const FreeListPool&) = delete;
    FreeListPool(FreeListPool&&) = delete;
    FreeListPool& operator=(FreeListPool&&) = delete;

    template <typename... Args>
    T* allocate(Args&&... args) {
        if (!free_head_) {
            return nullptr;
        }
        Slot* slot = free_head_;
        free_head_ = slot->next;
        --available_;
        return ::new (static_cast<void*>(slot->storage)) T(std::forward<Args>(args)...);
    }

    void deallocate(T* ptr) noexcept {
        if (!ptr) return;
        ptr->~T();
        auto* slot = reinterpret_cast<Slot*>(ptr);
        slot->next = free_head_;
        free_head_ = slot;
        ++available_;
    }

    [[nodiscard]] std::size_t available() const noexcept {
        return available_;
    }

    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return Capacity;
    }

private:
    union Slot {
        alignas(T) std::byte storage[sizeof(T)];
        Slot* next;
    };

    Slot slots_[Capacity];
    Slot* free_head_{nullptr};
    std::size_t available_{Capacity};
};

template <std::size_t Capacity>
class PoolSpawner {
public:
    Projectile* spawn(float x, float y, float vx, float vy, int ttl) {
        auto* p = pool_.allocate();
        if (!p) return nullptr;
        p->reset(x, y, vx, vy, ttl);
        active_projectiles_.push_back(p);
        return p;
    }

    void update(float dt) {
        for (std::size_t i = 0; i < active_projectiles_.size();) {
            auto* p = active_projectiles_[i];
            p->update(dt);
            if (!p->active) {
                pool_.deallocate(p);
                active_projectiles_[i] = active_projectiles_.back();
                active_projectiles_.pop_back();
            } else {
                ++i;
            }
        }
    }

    void clear() {
        for (auto* p : active_projectiles_) {
            pool_.deallocate(p);
        }
        active_projectiles_.clear();
    }

    [[nodiscard]] std::size_t active_count() const noexcept {
        return active_projectiles_.size();
    }

    [[nodiscard]] std::size_t pool_available() const noexcept {
        return pool_.available();
    }

private:
    FreeListPool<Projectile, Capacity> pool_;
    std::vector<Projectile*> active_projectiles_;
};

} // namespace memory_pool
