#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <list>
#include <utility>
#include <vector>

namespace vector_vs_list {

struct Entity {
    std::uint32_t id;
    float x;
    float y;
    float vx;
    float vy;

    void update(float dt) noexcept {
        x += vx * dt;
        y += vy * dt;
    }
};

class ListWorld {
public:
    void spawn(std::uint32_t id, float x, float y, float vx, float vy) {
        entities_.push_back(Entity{id, x, y, vx, vy});
    }

    bool remove_at(std::size_t index) {
        if (index >= entities_.size()) {
            return false;
        }
        auto it = entities_.begin();
        std::advance(it, static_cast<std::ptrdiff_t>(index));
        entities_.erase(it);
        return true;
    }

    void update_all(float dt) noexcept {
        for (auto& e : entities_) {
            e.update(dt);
        }
    }

    [[nodiscard]] std::size_t size() const noexcept { return entities_.size(); }
    [[nodiscard]] const std::list<Entity>& items() const noexcept { return entities_; }

private:
    std::list<Entity> entities_;
};

class DequeWorld {
public:
    void spawn(std::uint32_t id, float x, float y, float vx, float vy) {
        entities_.push_back(Entity{id, x, y, vx, vy});
    }

    bool remove_at(std::size_t index) {
        if (index >= entities_.size()) {
            return false;
        }
        if (index + 1 < entities_.size()) {
            entities_[index] = std::move(entities_.back());
        }
        entities_.pop_back();
        return true;
    }

    void update_all(float dt) noexcept {
        for (auto& e : entities_) {
            e.update(dt);
        }
    }

    [[nodiscard]] std::size_t size() const noexcept { return entities_.size(); }
    [[nodiscard]] const std::deque<Entity>& items() const noexcept { return entities_; }

private:
    std::deque<Entity> entities_;
};

class VectorSwapPopWorld {
public:
    explicit VectorSwapPopWorld(std::size_t reserve_capacity = 0) {
        if (reserve_capacity > 0) {
            entities_.reserve(reserve_capacity);
        }
    }

    void spawn(std::uint32_t id, float x, float y, float vx, float vy) {
        entities_.push_back(Entity{id, x, y, vx, vy});
    }

    bool remove_at(std::size_t index) noexcept {
        if (index >= entities_.size()) {
            return false;
        }
        if (index + 1 < entities_.size()) {
            entities_[index] = std::move(entities_.back());
        }
        entities_.pop_back();
        return true;
    }

    void update_all(float dt) noexcept {
        for (auto& e : entities_) {
            e.update(dt);
        }
    }

    [[nodiscard]] std::size_t size() const noexcept { return entities_.size(); }
    [[nodiscard]] const Entity* data() const noexcept { return entities_.data(); }
    [[nodiscard]] const std::vector<Entity>& items() const noexcept { return entities_; }

private:
    std::vector<Entity> entities_;
};

}  // namespace vector_vs_list
