#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

namespace static_dispatch {

template <int Tag>
inline void apply_behavior(float& x, float& y, float& vx, float& vy, float dt) noexcept {
    if constexpr (Tag == 0) {
        x += vx * dt;
        y += vy * dt;
    } else if constexpr (Tag == 1) {
        x += vx * dt * 1.5f;
        y += vy * dt * 0.5f;
    } else if constexpr (Tag == 2) {
        vx *= 0.999f;
        x += vx * dt;
        y += vy * dt;
    } else if constexpr (Tag == 3) {
        vy -= 9.81f * dt * 0.01f;
        x += vx * dt;
        y += vy * dt;
    } else if constexpr (Tag == 4) {
        x += (vx + vy) * dt;
        y += (vy - vx) * dt;
    } else if constexpr (Tag == 5) {
        x += vx * dt * 2.0f;
        y -= vy * dt;
    } else if constexpr (Tag == 6) {
        vx += 0.1f * dt;
        x += vx * dt;
        y += vy * dt;
    } else {
        x += vx * dt * 0.75f;
        y += vy * dt * 1.25f;
    }
}

// 1. Virtual Dispatch
struct IEntity {
    virtual ~IEntity() = default;
    virtual void update(float dt) noexcept = 0;
    [[nodiscard]] virtual double energy() const noexcept = 0;
};

template <int Tag>
struct VirtualEntity final : IEntity {
    float x, y, vx, vy;
    VirtualEntity(float x_, float y_, float vx_, float vy_) noexcept
        : x(x_), y(y_), vx(vx_), vy(vy_) {}
    void update(float dt) noexcept override { apply_behavior<Tag>(x, y, vx, vy, dt); }
    [[nodiscard]] double energy() const noexcept override { return static_cast<double>(x + y); }
};

class VirtualWorld {
public:
    explicit VirtualWorld(std::size_t count) {
        entities_.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 100) * 0.1f;
            entities_.push_back(make_tagged(static_cast<int>(i % 8), f, f * 0.5f, 1.5f, -0.5f));
        }
        if (entities_.size() > 1) {
            std::mt19937 rng(42);
            std::shuffle(entities_.begin(), entities_.end(), rng);
        }
    }

    void update_all(float dt, int frames) noexcept {
        for (int f = 0; f < frames; ++f) {
            for (const auto& e : entities_) {
                e->update(dt);
            }
        }
    }

    [[nodiscard]] double checksum() const noexcept {
        double s = 0.0;
        for (const auto& e : entities_) s += e->energy();
        return s;
    }

private:
    static std::unique_ptr<IEntity> make_tagged(int tag, float x, float y, float vx, float vy) {
        switch (tag) {
            case 0: return std::make_unique<VirtualEntity<0>>(x, y, vx, vy);
            case 1: return std::make_unique<VirtualEntity<1>>(x, y, vx, vy);
            case 2: return std::make_unique<VirtualEntity<2>>(x, y, vx, vy);
            case 3: return std::make_unique<VirtualEntity<3>>(x, y, vx, vy);
            case 4: return std::make_unique<VirtualEntity<4>>(x, y, vx, vy);
            case 5: return std::make_unique<VirtualEntity<5>>(x, y, vx, vy);
            case 6: return std::make_unique<VirtualEntity<6>>(x, y, vx, vy);
            default: return std::make_unique<VirtualEntity<7>>(x, y, vx, vy);
        }
    }
    std::vector<std::unique_ptr<IEntity>> entities_;
};

// Shared POD Entity for FnTable & TagSwitch
struct PodEntity {
    float x, y, vx, vy;
    std::uint8_t tag;
};

}  // namespace static_dispatch
