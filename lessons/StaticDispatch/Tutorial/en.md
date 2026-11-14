# Static Dispatch (Virtual vs CRTP vs Function Tables)

## The Cost of Polymorphic `virtual` Loops
Calling `e->update(dt)` through `std::vector<std::unique_ptr<IEntity>>` pays three penalties on every frame:
1. Heap pointer chasing across scattered `unique_ptr` instances
2. Vtable pointer dereference + indirect call preventing compiler inlining
3. CPU branch-target mispredictions when 8 behavior types are interleaved

## Homogeneous CRTP Buckets
Grouping entities by static type into `std::tuple<std::vector<CrtpEntity<0>>, ...>` using CRTP (`CrtpBase<Derived>`) eliminates both vtable indirection and branch mispredictions—running `8.62x` faster (`4.381 ms` vs `37.782 ms`) across `50,000` entities over `60` frames.

## Key Takeaways
- Contiguous storage alone (`TagSwitch` / `FnTable`) only recovers `1.19x`–`1.30x` if heterogeneous behaviors remain randomly interleaved.
- Sorting or bucketing entities by behavior type with static dispatch (`CRTP`) unlocks full inlining and `8.62x` speedup.
