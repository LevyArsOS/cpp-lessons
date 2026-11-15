# String View, SSO, and String Interning

## Problem
Game loops frequently resolve asset keys, entity tags, or localization identifiers formatted like `"combat_ns:entity_attribute_modifier_value_42"`. When a lookup slices a string via `std::string::substr` and searches an `std::unordered_map<std::string, T>`, any substring longer than the compiler's Small String Optimization (SSO) capacity (15 bytes on `libstdc++`) allocates and frees a temporary heap buffer on every query.

## Solution
1. **Use `std::string_view` with heterogeneous lookup (`is_transparent`):** `std::string_view::substr` is an $O(1)$ pointer-and-length adjustment that never allocates. Adding a transparent hash functor (`using is_transparent = void`) allows `std::unordered_map` to query a `std::string_view` directly without constructing a temporary `std::string`.
2. **Intern strings to `uint32_t` IDs at load time:** For hot loops executed thousands of times per frame, convert string keys into contiguous integer IDs once during initialization and index a flat `std::vector` at runtime.

## Benchmark Summary
Across `10,000` queries over `200` frames (`g++ -O2`), `std::string_view` transparent lookup cuts execution time from `86.858 ms` to `39.524 ms` (`2.20x` faster, `0` heap allocations), while pre-interned `uint32_t` IDs finish in `0.900 ms` (`96.47x` faster).
