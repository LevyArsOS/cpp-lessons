#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

namespace contiguous_objects {

struct Particle {
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    float life;
    float mass;

    void step(float dt) noexcept {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
        life -= dt;
    }
};

class PointerParticleSystem {
public:
    explicit PointerParticleSystem(std::size_t count, bool scatter_heap = true) {
        particles_.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 100) * 0.1f;
            particles_.push_back(std::make_unique<Particle>(Particle{
                f, f * 0.5f, 0.0f, 1.5f, -0.5f, 0.25f, 10.0f, 1.0f
            }));
        }
        if (scatter_heap && particles_.size() > 1) {
            std::mt19937 rng(42);
            std::shuffle(particles_.begin(), particles_.end(), rng);
        }
    }

    void step(float dt, int frames) noexcept {
        for (int f = 0; f < frames; ++f) {
            for (const auto& p : particles_) {
                p->step(dt);
            }
        }
    }

    [[nodiscard]] double checksum() const noexcept {
        double sum = 0.0;
        for (const auto& p : particles_) {
            sum += static_cast<double>(p->x + p->y + p->z + p->life);
        }
        return sum;
    }

    [[nodiscard]] std::size_t size() const noexcept { return particles_.size(); }

private:
    std::vector<std::unique_ptr<Particle>> particles_;
};

class ContiguousParticleSystem {
public:
    explicit ContiguousParticleSystem(std::size_t count) {
        particles_.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 100) * 0.1f;
            particles_.push_back(Particle{
                f, f * 0.5f, 0.0f, 1.5f, -0.5f, 0.25f, 10.0f, 1.0f
            });
        }
    }

    void step(float dt, int frames) noexcept {
        for (int f = 0; f < frames; ++f) {
            for (auto& p : particles_) {
                p.step(dt);
            }
        }
    }

    [[nodiscard]] double checksum() const noexcept {
        double sum = 0.0;
        for (const auto& p : particles_) {
            sum += static_cast<double>(p.x + p.y + p.z + p.life);
        }
        return sum;
    }

    [[nodiscard]] std::size_t size() const noexcept { return particles_.size(); }
    [[nodiscard]] const Particle* data() const noexcept { return particles_.data(); }
    [[nodiscard]] const std::vector<Particle>& items() const noexcept { return particles_; }

private:
    std::vector<Particle> particles_;
};

}  // namespace contiguous_objects
