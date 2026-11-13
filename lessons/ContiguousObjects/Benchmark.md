# Contiguous Objects Benchmark

## Environment
- Compiler: `g++ 13.3.0` (`-std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror`)
- CPU: AMD Ryzen 7 5800X (Linux x86_64)
- Workload: `100,000` `Particle` objects (`32` bytes each) stepped for `60` frames (`dt = 0.016f`)

## Results

| Layout | Total Time (`60` frames) | Speedup |
|---|---:|---:|
| `std::vector<std::unique_ptr<Particle>>` (scattered pointers) | `21.188 ms` | `1.00x` |
| `std::vector<Particle>` (contiguous values) | `7.357 ms` | `2.88x` faster |

## Analysis
1. **Pointer chasing tax**: Each `std::unique_ptr<Particle>` dereferences a separate heap address. Even with a compact 32-byte `Particle`, shuffled heap pointers break sequential hardware prefetching and stall the CPU waiting on L2/L3 cache lines.
2. **Contiguous cache lines**: `std::vector<Particle>` packs two 32-byte particles into every 64-byte CPU cache line with zero pointer indirection, cutting 60-frame update time by `65.3%` (`2.88x` faster).
