#pragma once

namespace memory_pool {

struct Projectile {
    float x{0.0f};
    float y{0.0f};
    float vx{0.0f};
    float vy{0.0f};
    int ttl{0};
    bool active{false};

    void update(float dt) noexcept {
        if (!active) return;
        x += vx * dt;
        y += vy * dt;
        --ttl;
        if (ttl <= 0) {
            active = false;
        }
    }

    void reset(float initial_x, float initial_y, float initial_vx, float initial_vy, int initial_ttl) noexcept {
        x = initial_x;
        y = initial_y;
        vx = initial_vx;
        vy = initial_vy;
        ttl = initial_ttl;
        active = true;
    }
};

} // namespace memory_pool
