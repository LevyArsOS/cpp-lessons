#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace string_view_sso {

inline std::vector<std::string> make_sample_queries(std::size_t count) {
    std::vector<std::string> queries;
    queries.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        std::size_t id = i % 64;
        // Key suffix exceeds 15 bytes so std::string::substr forces a heap allocation beyond SSO
        queries.push_back("combat_ns:entity_attribute_modifier_value_" + std::to_string(id));
    }
    return queries;
}

// 1. Naive: slices composite keys into temporary std::string objects on every lookup
class NaiveStringCatalog {
public:
    NaiveStringCatalog() {
        for (int i = 0; i < 64; ++i) {
            std::string key = "entity_attribute_modifier_value_" + std::to_string(i);
            table_.emplace(std::move(key), (i + 1) * 10);
        }
    }

    [[nodiscard]] int lookup(const std::string& composite_key) const {
        std::size_t colon = composite_key.find(':');
        if (colon == std::string::npos || colon + 1 >= composite_key.size()) {
            return -1;
        }
        std::string sub_key = composite_key.substr(colon + 1);
        auto it = table_.find(sub_key);
        return it != table_.end() ? it->second : -1;
    }

    [[nodiscard]] std::int64_t resolve_batch(const std::vector<std::string>& queries, int frames) const {
        std::int64_t sum = 0;
        for (int f = 0; f < frames; ++f) {
            for (const auto& q : queries) {
                sum += lookup(q);
            }
        }
        return sum;
    }

private:
    std::unordered_map<std::string, int> table_;
};

// Transparent hash and equality for zero-allocation std::string_view lookup
struct TransparentStringHash {
    using is_transparent = void;
    [[nodiscard]] std::size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
};

// 2. StringView: slices with std::string_view (pointer + length, zero heap allocations)
class StringViewCatalog {
public:
    StringViewCatalog() {
        for (int i = 0; i < 64; ++i) {
            std::string key = "entity_attribute_modifier_value_" + std::to_string(i);
            table_.emplace(std::move(key), (i + 1) * 10);
        }
    }

    [[nodiscard]] int lookup(std::string_view composite_key) const noexcept {
        std::size_t colon = composite_key.find(':');
        if (colon == std::string_view::npos || colon + 1 >= composite_key.size()) {
            return -1;
        }
        std::string_view sub_key = composite_key.substr(colon + 1);
        auto it = table_.find(sub_key);
        return it != table_.end() ? it->second : -1;
    }

    [[nodiscard]] std::int64_t resolve_batch(const std::vector<std::string>& queries, int frames) const noexcept {
        std::int64_t sum = 0;
        for (int f = 0; f < frames; ++f) {
            for (const auto& q : queries) {
                sum += lookup(q);
            }
        }
        return sum;
    }

private:
    std::unordered_map<std::string, int, TransparentStringHash, std::equal_to<>> table_;
};

// 3. Interned IDs: resolves string keys to uint32_t handles at load time, indexing a flat vector at runtime
class InternedCatalog {
public:
    static constexpr std::uint32_t kInvalidId = 0xFFFFFFFFu;

    InternedCatalog() {
        values_.resize(64);
        for (std::uint32_t i = 0; i < 64; ++i) {
            std::string key = "entity_attribute_modifier_value_" + std::to_string(i);
            key_to_id_.emplace(std::move(key), i);
            values_[i] = static_cast<int>((i + 1) * 10);
        }
    }

    [[nodiscard]] std::uint32_t intern_composite(std::string_view composite_key) const noexcept {
        std::size_t colon = composite_key.find(':');
        if (colon == std::string_view::npos || colon + 1 >= composite_key.size()) {
            return kInvalidId;
        }
        std::string_view sub_key = composite_key.substr(colon + 1);
        auto it = key_to_id_.find(sub_key);
        return it != key_to_id_.end() ? it->second : kInvalidId;
    }

    [[nodiscard]] std::vector<std::uint32_t> preintern_queries(const std::vector<std::string>& queries) const {
        std::vector<std::uint32_t> ids;
        ids.reserve(queries.size());
        for (const auto& q : queries) {
            ids.push_back(intern_composite(q));
        }
        return ids;
    }

    [[nodiscard]] int lookup_id(std::uint32_t id) const noexcept {
        return id < values_.size() ? values_[id] : -1;
    }

    [[nodiscard]] std::int64_t resolve_batch(const std::vector<std::uint32_t>& query_ids, int frames) const noexcept {
        std::int64_t sum = 0;
        for (int f = 0; f < frames; ++f) {
            for (std::uint32_t id : query_ids) {
                sum += lookup_id(id);
            }
        }
        return sum;
    }

private:
    std::unordered_map<std::string, std::uint32_t, TransparentStringHash, std::equal_to<>> key_to_id_;
    std::vector<int> values_;
};

}  // namespace string_view_sso
