# Benchmark Results - Memory Pool vs new/delete

This document presents benchmark results comparing standard heap allocation (`new`/`delete`), an intrusive `FreeListPool`, and a linear `ArenaAllocator` across 50,000 projectile spawn/update/free cycles.

## Test Environment

- **Compiler**: g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- **Flags**: `-std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror`
- **CPU**: AMD Ryzen 7 5800X (8 cores / 16 threads @ 3.79 GHz)
- **OS**: Linux x86_64 (Ubuntu 24.04 LTS)

---

## 50,000 Projectile Lifecycle Operations

### Comparative Table

| Allocator Strategy | Total Time | Latency per Op | Ratio vs Baseline | Steady-State Heap Allocs |
|---|---|---|---|---|
| **Naive `new` / `delete`** (Baseline) | 0.89 ms | 17.81 ns/op | 1.00x | 50,000 |
| **`FreeListPool`** | 0.29 ms | 5.71 ns/op | 0.32x | **0** |
| **`ArenaAllocator`** | 0.24 ms | 4.86 ns/op | 0.27x | **0** |

### Analysis

1. **Throughput and Latency**:
   - `FreeListPool` achieves a **3.1x speedup** (5.71 ns/op vs 17.81 ns/op) over naive dynamic allocation.
   - `ArenaAllocator` achieves a **3.7x speedup** (4.86 ns/op), offering the lowest latency due to simple pointer bumping without list traversal.

2. **Heap Allocations and Fragmentation**:
   - The naive approach executes **50,000 individual heap allocations and deallocations**, causing memory fragmentation, metadata overhead (16 bytes per glibc chunk), and cache misses.
   - Both `FreeListPool` and `ArenaAllocator` run with **zero steady-state heap allocations** after initial buffer instantiation.

3. **Cache Efficiency**:
   - Pre-allocated pools keep active objects within contiguous cache lines, drastically lowering L1/L2 data cache misses during entity iteration.
