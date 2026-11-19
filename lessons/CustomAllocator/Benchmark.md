# CustomAllocator Benchmark

## Environment
- Compiler: `g++ 13.3.0` (`-std=c++20 -O2`), CMake Release build (`./build/CustomAllocator_benchmark`)
- Workload: 1,000 frames x 64 temporary dynamic `vector<DrawCommand>` instances x 24 `push_back` items per vector

## Results

| Allocator | Time (ms) | Speedup | Checksum |
|---|---:|---:|---:|
| `std::vector` (default heap) | 7.930 | 1.00x | 4867584000 |
| `std::pmr::unsynchronized_pool_resource` | 9.564 | 0.83x | 4867584000 |
| `std::pmr::monotonic_buffer_resource` + `release()` | **4.908** | **1.62x** | 4867584000 |

## Key Takeaways
- `std::pmr::monotonic_buffer_resource` backed by a 64 KB stack buffer and reset via `arena_.release()` at the end of each frame runs **1.62x faster** (`4.908 ms` vs `7.930 ms`) with **zero** upstream `::operator new`/`::operator delete` calls.
- `std::pmr::unsynchronized_pool_resource` (`9.564 ms`) has higher per-allocation bookkeeping than `monotonic_buffer_resource` for pure bump-and-reset scratch lifetimes, so it should be reserved for intra-frame churn where containers are created and destroyed repeatedly *without* a frame-end `release()`.
