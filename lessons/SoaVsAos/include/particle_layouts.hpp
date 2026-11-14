#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace soa_vs_aos {

struct CullResult {
    std::size_t visible_count;
    double checksum;
};

struct AosParticle {
    // Hot physics channels (24 bytes)
    float x, y, z;
    float vx, vy, vz;
    // Cold rendering/metadata channels (104 bytes -> 128B total struct, 2 cache lines)
    float life;
    float r, g, b, a;
    float cold_render_matrix[16];
    std::uint32_t flags;
    std::uint32_t pad[4];
};

static_assert(sizeof(AosParticle) == 128, "AosParticle should span two 64-byte cache lines");

class AosParticles {
public:
    explicit AosParticles(std::size_t count) {
        particles_.resize(count);
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 200) * 0.1f;
            particles_[i] = AosParticle{
                f, f * 0.5f, -f * 0.25f,
                1.5f, -0.5f, 0.75f,
                10.0f, 1.0f, 0.8f, 0.2f, 1.0f,
                {}, 1u, {0u, 0u, 0u, 0u}
            };
        }
    }

    CullResult integrate_and_cull(float dt, float max_bound, int frames) noexcept {
        for (int f = 0; f < frames; ++f) {
            for (auto& p : particles_) {
                p.x += p.vx * dt;
                p.y += p.vy * dt;
                p.z += p.vz * dt;
            }
        }

        std::size_t visible = 0;
        double sum = 0.0;
        for (const auto& p : particles_) {
            if (p.x >= -max_bound && p.x <= max_bound &&
                p.y >= -max_bound && p.y <= max_bound &&
                p.z >= -max_bound && p.z <= max_bound) {
                ++visible;
                sum += static_cast<double>(p.x + p.y + p.z);
            }
        }
        return CullResult{visible, sum};
    }

    [[nodiscard]] std::size_t size() const noexcept { return particles_.size(); }

private:
    std::vector<AosParticle> particles_;
};

class SoaParticles {
public:
    explicit SoaParticles(std::size_t count)
        : x_(count), y_(count), z_(count),
          vx_(count), vy_(count), vz_(count),
          life_(count), r_(count), g_(count), b_(count), a_(count), flags_(count) {
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 200) * 0.1f;
            x_[i] = f;
            y_[i] = f * 0.5f;
            z_[i] = -f * 0.25f;
            vx_[i] = 1.5f;
            vy_[i] = -0.5f;
            vz_[i] = 0.75f;
            life_[i] = 10.0f;
            r_[i] = 1.0f;
            g_[i] = 0.8f;
            b_[i] = 0.2f;
            a_[i] = 1.0f;
            flags_[i] = 1u;
        }
    }

    CullResult integrate_and_cull(float dt, float max_bound, int frames) noexcept {
        const std::size_t n = x_.size();
        float* __restrict px = x_.data();
        float* __restrict py = y_.data();
        float* __restrict pz = z_.data();
        const float* __restrict pvx = vx_.data();
        const float* __restrict pvy = vy_.data();
        const float* __restrict pvz = vz_.data();

        for (int f = 0; f < frames; ++f) {
            for (std::size_t i = 0; i < n; ++i) {
                px[i] += pvx[i] * dt;
                py[i] += pvy[i] * dt;
                pz[i] += pvz[i] * dt;
            }
        }

        std::size_t visible = 0;
        double sum = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            float x = px[i];
            float y = py[i];
            float z = pz[i];
            if (x >= -max_bound && x <= max_bound &&
                y >= -max_bound && y <= max_bound &&
                z >= -max_bound && z <= max_bound) {
                ++visible;
                sum += static_cast<double>(x + y + z);
            }
        }
        return CullResult{visible, sum};
    }

    [[nodiscard]] std::size_t size() const noexcept { return x_.size(); }

private:
    // Hot physics channels
    std::vector<float> x_, y_, z_;
    std::vector<float> vx_, vy_, vz_;
    // Cold rendering/metadata channels
    std::vector<float> life_, r_, g_, b_, a_;
    std::vector<std::uint32_t> flags_;
};

}  // namespace soa_vs_aos
