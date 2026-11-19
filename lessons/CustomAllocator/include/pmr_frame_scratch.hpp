#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <vector>

namespace cpp_lessons::custom_allocator {

struct DrawCommand {
    uint32_t entity_id{};
    float x{};
    float y{};
    uint32_t shader_key{};
};

/// Custom std::pmr::memory_resource tracking upstream allocations and byte totals.
class CountingResource final : public std::pmr::memory_resource {
public:
    explicit CountingResource(std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : upstream_(upstream) {}

    [[nodiscard]] size_t allocations() const noexcept { return allocations_; }
    [[nodiscard]] size_t deallocations() const noexcept { return deallocations_; }
    [[nodiscard]] size_t bytes_allocated() const noexcept { return bytes_allocated_; }

    void reset_counters() noexcept {
        allocations_ = 0;
        deallocations_ = 0;
        bytes_allocated_ = 0;
    }

private:
    void* do_allocate(size_t bytes, size_t alignment) override {
        ++allocations_;
        bytes_allocated_ += bytes;
        return upstream_->allocate(bytes, alignment);
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        ++deallocations_;
        upstream_->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    std::pmr::memory_resource* upstream_;
    size_t allocations_{0};
    size_t deallocations_{0};
    size_t bytes_allocated_{0};
};

template <typename Vec>
uint64_t fill_commands(Vec& cmds, uint32_t bucket, uint32_t count) {
    uint64_t sum = 0;
    for (uint32_t i = 0; i < count; ++i) {
        const DrawCommand cmd{
            (bucket * 100u) + i,
            static_cast<float>(i),
            static_cast<float>(bucket),
            (bucket + i) & 15u
        };
        cmds.push_back(cmd);
        sum += cmd.entity_id + cmd.shader_key;
    }
    return sum;
}

/// Baseline: temporary std::vector<DrawCommand> backed by global ::operator new/delete.
class HeapFrameBuilder {
public:
    uint64_t run_frame(uint32_t buckets, uint32_t items_per_bucket) {
        uint64_t checksum = 0;
        for (uint32_t b = 0; b < buckets; ++b) {
            std::vector<DrawCommand> cmds;
            checksum += fill_commands(cmds, b, items_per_bucket);
        }
        return checksum;
    }
};

/// Optimized (scratch): monotonic_buffer_resource backed by a pre-allocated buffer + O(1) release().
class MonotonicFrameBuilder {
public:
    explicit MonotonicFrameBuilder(std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : arena_(buffer_.data(), buffer_.size(), upstream) {}

    uint64_t run_frame(uint32_t buckets, uint32_t items_per_bucket) {
        uint64_t checksum = 0;
        for (uint32_t b = 0; b < buckets; ++b) {
            std::pmr::vector<DrawCommand> cmds(&arena_);
            checksum += fill_commands(cmds, b, items_per_bucket);
        }
        arena_.release();
        return checksum;
    }

private:
    std::array<std::byte, 65536> buffer_{};
    std::pmr::monotonic_buffer_resource arena_;
};

/// Optimized (intra-frame churn): unsynchronized_pool_resource recycles freed size-class blocks.
class PoolFrameBuilder {
public:
    explicit PoolFrameBuilder(std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : pool_(upstream) {}

    uint64_t run_frame(uint32_t buckets, uint32_t items_per_bucket) {
        uint64_t checksum = 0;
        for (uint32_t b = 0; b < buckets; ++b) {
            std::pmr::vector<DrawCommand> cmds(&pool_);
            checksum += fill_commands(cmds, b, items_per_bucket);
        }
        return checksum;
    }

private:
    std::pmr::unsynchronized_pool_resource pool_;
};

}  // namespace cpp_lessons::custom_allocator
