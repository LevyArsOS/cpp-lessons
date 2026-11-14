# SoA vs AoS (Particle Physics & Frustum Culling)

## Why AoS Pollutes Cache Lines
In an Array of Structures (`AoS`), hot physics fields (`x, y, z, vx, vy, vz`) sit next to cold rendering fields (`color`, `transform`, `flags`). When the physics or culling pass streams `500,000` particles, the CPU loads entire 128-byte structs into cache lines even though only 24 bytes are needed—wasting ~81% of memory bandwidth.

## Structure of Arrays (`SoA`)
Splitting hot channels into parallel contiguous `std::vector<float>` arrays packs 16 coordinates per 64-byte cache line and lets the compiler auto-vectorize the loop:

```cpp
// AoS: 228.95 ms (loads 104B of cold render fields per particle)
for (auto& p : particles_) {
    p.x += p.vx * dt; p.y += p.vy * dt; p.z += p.vz * dt;
}

// SoA: 31.53 ms (7.26x faster, 100% hot cache-line utilization)
for (std::size_t i = 0; i < n; ++i) {
    px[i] += pvx[i] * dt; py[i] += pvy[i] * dt; pz[i] += pvz[i] * dt;
}
```

## Key Takeaways
- Separate hot per-frame channels from cold rendering/UI metadata when dataset size exceeds L3 cache.
- Use `__restrict` on raw channel pointers in C++ to help the compiler prove non-aliasing for SIMD vectorization.
