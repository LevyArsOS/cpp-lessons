#pragma once

#include "dispatch_common.hpp"

#include <array>
#include <tuple>
#include <utility>

namespace static_dispatch {

// 2. Function Pointer Table Dispatch
class FnTableWorld {
public:
    explicit FnTableWorld(std::size_t count) {
        entities_.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 100) * 0.1f;
            entities_.push_back(PodEntity{f, f * 0.5f, 1.5f, -0.5f, static_cast<std::uint8_t>(i % 8)});
        }
        if (entities_.size() > 1) {
            std::mt19937 rng(42);
            std::shuffle(entities_.begin(), entities_.end(), rng);
        }
    }

    void update_all(float dt, int frames) noexcept {
        static constexpr std::array<void (*)(PodEntity&, float) noexcept, 8> table = {
            &step<0>, &step<1>, &step<2>, &step<3>, &step<4>, &step<5>, &step<6>, &step<7>
        };
        for (int f = 0; f < frames; ++f) {
            for (auto& e : entities_) {
                table[e.tag & 7u](e, dt);
            }
        }
    }

    [[nodiscard]] double checksum() const noexcept {
        double s = 0.0;
        for (const auto& e : entities_) s += static_cast<double>(e.x + e.y);
        return s;
    }

private:
    template <int Tag>
    static void step(PodEntity& e, float dt) noexcept {
        apply_behavior<Tag>(e.x, e.y, e.vx, e.vy, dt);
    }
    std::vector<PodEntity> entities_;
};

// 3. Tag Switch Dispatch (contiguous, inlineable)
class TagSwitchWorld {
public:
    explicit TagSwitchWorld(std::size_t count) {
        entities_.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 100) * 0.1f;
            entities_.push_back(PodEntity{f, f * 0.5f, 1.5f, -0.5f, static_cast<std::uint8_t>(i % 8)});
        }
        if (entities_.size() > 1) {
            std::mt19937 rng(42);
            std::shuffle(entities_.begin(), entities_.end(), rng);
        }
    }

    void update_all(float dt, int frames) noexcept {
        for (int f = 0; f < frames; ++f) {
            for (auto& e : entities_) {
                switch (e.tag) {
                    case 0: apply_behavior<0>(e.x, e.y, e.vx, e.vy, dt); break;
                    case 1: apply_behavior<1>(e.x, e.y, e.vx, e.vy, dt); break;
                    case 2: apply_behavior<2>(e.x, e.y, e.vx, e.vy, dt); break;
                    case 3: apply_behavior<3>(e.x, e.y, e.vx, e.vy, dt); break;
                    case 4: apply_behavior<4>(e.x, e.y, e.vx, e.vy, dt); break;
                    case 5: apply_behavior<5>(e.x, e.y, e.vx, e.vy, dt); break;
                    case 6: apply_behavior<6>(e.x, e.y, e.vx, e.vy, dt); break;
                    default: apply_behavior<7>(e.x, e.y, e.vx, e.vy, dt); break;
                }
            }
        }
    }

    [[nodiscard]] double checksum() const noexcept {
        double s = 0.0;
        for (const auto& e : entities_) s += static_cast<double>(e.x + e.y);
        return s;
    }

private:
    std::vector<PodEntity> entities_;
};

// 4. CRTP Homogeneous Buckets
template <typename Derived>
struct CrtpBase {
    void update(float dt) noexcept { static_cast<Derived*>(this)->update_impl(dt); }
};

template <int Tag>
struct CrtpEntity : CrtpBase<CrtpEntity<Tag>> {
    float x, y, vx, vy;
    void update_impl(float dt) noexcept { apply_behavior<Tag>(x, y, vx, vy, dt); }
};

class CrtpWorld {
public:
    explicit CrtpWorld(std::size_t count) {
        for (std::size_t i = 0; i < count; ++i) {
            float f = static_cast<float>(i % 100) * 0.1f;
            add_by_tag(static_cast<int>(i % 8), f, f * 0.5f, 1.5f, -0.5f);
        }
    }

    void update_all(float dt, int frames) noexcept {
        for (int f = 0; f < frames; ++f) {
            std::apply([dt](auto&... vecs) { (update_bucket(vecs, dt), ...); }, buckets_);
        }
    }

    [[nodiscard]] double checksum() const noexcept {
        double s = 0.0;
        std::apply([&s](const auto&... vecs) { ((s += sum_bucket(vecs)), ...); }, buckets_);
        return s;
    }

private:
    template <typename Vec>
    static void update_bucket(Vec& v, float dt) noexcept {
        for (auto& e : v) e.update(dt);
    }

    template <typename Vec>
    static double sum_bucket(const Vec& v) noexcept {
        double s = 0.0;
        for (const auto& e : v) s += static_cast<double>(e.x + e.y);
        return s;
    }

    void add_by_tag(int tag, float x, float y, float vx, float vy) {
        switch (tag) {
            case 0: std::get<0>(buckets_).push_back({{}, x, y, vx, vy}); break;
            case 1: std::get<1>(buckets_).push_back({{}, x, y, vx, vy}); break;
            case 2: std::get<2>(buckets_).push_back({{}, x, y, vx, vy}); break;
            case 3: std::get<3>(buckets_).push_back({{}, x, y, vx, vy}); break;
            case 4: std::get<4>(buckets_).push_back({{}, x, y, vx, vy}); break;
            case 5: std::get<5>(buckets_).push_back({{}, x, y, vx, vy}); break;
            case 6: std::get<6>(buckets_).push_back({{}, x, y, vx, vy}); break;
            default: std::get<7>(buckets_).push_back({{}, x, y, vx, vy}); break;
        }
    }

    std::tuple<
        std::vector<CrtpEntity<0>>, std::vector<CrtpEntity<1>>,
        std::vector<CrtpEntity<2>>, std::vector<CrtpEntity<3>>,
        std::vector<CrtpEntity<4>>, std::vector<CrtpEntity<5>>,
        std::vector<CrtpEntity<6>>, std::vector<CrtpEntity<7>>
    > buckets_;
};

}  // namespace static_dispatch
