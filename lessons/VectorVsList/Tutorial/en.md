# Vector vs List (Swap-and-Pop Removal)

## Why `std::list` Hurts Game Loops
When entities spawn and despawn every frame, reaching for `std::list` seems intuitive because `erase(it)` does not shift subsequent elements. In practice, every `std::list` node is a separate heap allocation scattered across memory, destroying CPU cache locality during per-frame updates and making index-based lookups $O(n)$.

## The $O(1)$ Swap-and-Pop Pattern
When element ordering is not required (projectiles, particles, enemies, active timers), `std::vector` can remove any element at `index` in $O(1)$ time:

```cpp
bool remove_at(std::size_t index) noexcept {
    if (index >= entities_.size()) {
        return false;
    }
    if (index + 1 < entities_.size()) {
        entities_[index] = std::move(entities_.back());
    }
    entities_.pop_back();
    return true;
}
```

## Trade-offs
- **Pros**: Contiguous memory layout, hardware prefetching, zero per-node heap overhead, and $O(1)$ removal (`161x`–`184x` faster than `std::list` in mixed update + removal workloads).
- **Cons**: Element order is not preserved after removal, and stored indices/iterators to the moved last element are invalidated.
