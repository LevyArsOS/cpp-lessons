# Vector vs List Benchmark

## Environment
- Compiler: `g++ 13.3.0` (`-std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror`)
- CPU: AMD Ryzen 7 5800X (Linux x86_64)
- Workload: `20` frames of full `update_all(0.016f)` iteration + `200` random removals/spawns per frame

## Results

| Entity Count (`N`) | `std::list` | `std::deque` | `std::vector` (swap-and-pop) | Speedup vs `std::list` |
|---|---|---|---|---|
| `10,000` | `30.302 ms` | `0.239 ms` | `0.165 ms` | `184.20x` faster |
| `100,000` | `233.821 ms` | `3.044 ms` | `1.451 ms` | `161.17x` faster |

## Analysis
1. **`std::list` node chasing**: Every `Entity` lives in a separately allocated heap node with two 8-byte pointers (`prev`/`next`). Iterating 100,000 entities and advancing iterators for random removals triggers constant L1/L2 CPU cache misses.
2. **`std::deque` chunked blocks**: `std::deque` avoids per-element heap nodes by storing fixed-size blocks, making it ~76x faster than `std::list` at `100,000` entities, yet block indirection still lags a single contiguous buffer (`3.044 ms` vs `1.451 ms`).
3. **`std::vector` with swap-and-pop**: Swapping the target element with `back()` and calling `pop_back()` runs in $O(1)$ without shifting elements, while keeping the entire active array contiguous for hardware prefetching (`1.451 ms`, `161.17x` faster than `std::list`).
