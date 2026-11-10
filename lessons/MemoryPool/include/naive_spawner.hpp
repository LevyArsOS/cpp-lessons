#pragma once

#include "projectile.hpp"
#include <vector>

namespace memory_pool {

class NaiveSpawner {
public:
    ~NaiveSpawner() {
        clear();
    }

    Projectile* spawn(float x, float y, float vx, float vy, int ttl) {
        auto* p = new Projectile();
        p->reset(x, y, vx, vy, ttl);
        active_projectiles_.push_back(p);
        return p;
    }

    void update(float dt) {
        for (std::size_t i = 0; i < active_projectiles_.size();) {
            auto* p = active_projectiles_[i];
            p->update(dt);
            if (!p->active) {
                delete p;
                active_projectiles_[i] = active_projectiles_.back();
                active_projectiles_.pop_back();
            } else {
                ++i;
            }
        }
    }

    void clear() {
        for (auto* p : active_projectiles_) {
            delete p;
        }
        active_projectiles_.clear();
    }

    [[nodiscard]] std::size_t active_count() const noexcept {
        return active_projectiles_.size();
    }

private:
    std::vector<Projectile*> active_projectiles_;
};

} // namespace memory_pool
