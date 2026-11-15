# String View & SSO Benchmark Results

## Environment
- **Compiler:** `g++` (`-std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror`)
- **Workload:** `10,000` composite attribute queries (`"combat_ns:entity_attribute_modifier_value_<id>"`) per frame across `200` frames (`2,000,000` total lookups).

## Results

| Approach | Total Time (200 frames) | Speedup vs Baseline | Hot-Path Heap Allocations |
|---|---:|---:|---:|
| `NaiveStringCatalog` (`std::string::substr`) | `86.858 ms` | `1.00x` | `2,000,000` |
| `StringViewCatalog` (`std::string_view` + transparent hash) | `39.524 ms` | `2.20x` | `0` |
| `InternedCatalog` (pre-interned `uint32_t` ID) | `0.900 ms` | `96.47x` | `0` |

## Analysis
1. **Exceeding Small String Optimization (SSO):** On `libstdc++` x86_64, `std::string` stores up to 15 characters inline. Slicing `"entity_attribute_modifier_value_<id>"` (> 32 bytes) into a temporary `std::string` forces a heap allocation and deallocation on every single query (`86.858 ms`).
2. **Non-owning `std::string_view` + Transparent Hash (`2.20x` faster):** Slicing a `std::string_view` only adjusts a `(ptr, len)` pair in registers. Combined with `using is_transparent = void` on the hash functor, `std::unordered_map::find` hashes the slice in-place with zero heap allocations (`39.524 ms`).
3. **String Interning (`96.47x` faster):** Resolving strings to dense `uint32_t` IDs at load time eliminates per-frame string hashing and byte comparisons altogether, turning every hot-path lookup into a `0.900 ms` flat `std::vector<int>` index access.
