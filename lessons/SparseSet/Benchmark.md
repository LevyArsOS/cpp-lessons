# SparseSet Benchmark

## Environment
- Compiler: `g++ 13.3.0` (`-std=c++20 -O2`), CMake Release build (`./build/SparseSet_benchmark`)
- Workload: 100,000 entity insertions, 100,000 shuffled lookups, 100-frame velocity iteration, and 50,000 shuffled erasures

## Results

| Container | Insert (ms) | Lookup (ms) | Iterate 100x (ms) | Erase (ms) | Total (ms) | Speedup vs `unordered_map` | Checksum |
|---|---:|---:|---:|---:|---:|---:|---:|
| `std::unordered_map` | 8.297 | 5.835 | 17.334 | 15.390 | 46.857 | 1.00x | 10101585.9 |
| `FlatOpenMap` | 5.437 | 4.310 | 111.301 | 0.813 | 121.861 | 0.38x | 10101585.9 |
| `SparseSet<T>` | **0.463** | **1.051** | **12.269** | **0.777** | **14.559** | **3.22x** | 10101585.9 |

## Key Takeaways
- `SparseSet<T>` achieves **17.92x faster insertion** (`0.463 ms` vs `8.297 ms`), **5.55x faster random lookup** (`1.051 ms` vs `5.835 ms`), and **19.81x faster erasure** (`0.777 ms` vs `15.390 ms`) than `std::unordered_map` because it avoids hashing, bucket chasing, and per-node `new`/`delete`.
- `FlatOpenMap` speeds up inserts, lookups, and erasures compared to `std::unordered_map`, but full iteration (`111.301 ms`) is slow because it must scan empty slots across a `262,144`-slot hash table.
- `SparseSet<T>` packs active components contiguously (`std::span<T>`), keeping iteration fast and dense regardless of `EntityId` sparsity.
