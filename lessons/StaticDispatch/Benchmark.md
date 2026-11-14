# Static Dispatch Benchmark

## Environment
- Compiler: `g++ 13.3.0` (`-std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror`)
- CPU: AMD Ryzen 7 5800X (Linux x86_64)
- Workload: `50,000` entities across `8` behavior types updated over `60` frames (`dt = 0.016f`)

## Results

| Dispatch Strategy | Total Time (`60` frames) | Speedup vs Virtual |
|---|---:|---:|
| `VirtualWorld` (vtable + shuffled `unique_ptr`) | `37.782 ms` | `1.00x` |
| `TagSwitchWorld` (contiguous `PodEntity`, `switch`) | `31.641 ms` | `1.19x` faster |
| `FnTableWorld` (contiguous `PodEntity`, function table) | `29.142 ms` | `1.30x` faster |
| `CrtpWorld` (8 homogeneous CRTP `std::vector` buckets) | `4.381 ms` | `8.62x` faster |

## Analysis
1. **Why shuffled `TagSwitch` and `FnTable` only gain `1.19x`–`1.30x`**: Even when objects are contiguous in `std::vector<PodEntity>`, calling 8 random behaviors on every entity still triggers branch-target mispredictions on `switch` / indirect function pointer calls.
2. **Why `CrtpWorld` homogeneous buckets reach `8.62x` faster (`4.381 ms`)**: Grouping entities by behavior type into contiguous `std::vector<CrtpEntity<Tag>>` buckets eliminates vtables, eliminates per-entity branch mispredictions, and lets the compiler inline and auto-vectorize each bucket loop cleanly.
