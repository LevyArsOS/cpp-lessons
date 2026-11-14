# SoA vs AoS Benchmark

## Environment
- Compiler: `g++ 13.3.0` (`-std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror`)
- CPU: AMD Ryzen 7 5800X (Linux x86_64)
- Workload: `500,000` particles (`24 B` hot physics + `104 B` cold render/metadata = `128 B` AoS struct) integrated and culled over `60` frames (`dt = 0.016f`)

## Results

| Layout | Total Time (`60` frames) | Speedup |
|---|---:|---:|
| `AoS` (`std::vector<AosParticle>`, `128 B` struct) | `228.953 ms` | `1.00x` |
| `SoA` (`std::vector<float>` hot channels) | `31.530 ms` | `7.26x` faster |

## Analysis
1. **Cold-field cache pollution (`AoS`)**: Each 128-byte `AosParticle` spans two 64-byte CPU cache lines, yet physics integration and bounds culling only read 24 bytes (`18.75%` cache-line utilization). Across `500,000` particles (`64 MB` > `32 MB` L3 cache), DRAM bandwidth is wasted streaming cold render matrices.
2. **100% hot cache-line utilization (`SoA`)**: Separate contiguous `std::vector<float>` arrays pack `16` `x` (or `y`/`z`) floats per 64-byte cache line (`12 MB` hot working set) and enable SIMD auto-vectorization, running `7.26x` faster (`31.530 ms` vs `228.953 ms`).
