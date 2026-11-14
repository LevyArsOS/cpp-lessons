#include "SoaVsAos/include/particle_layouts.hpp"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>

int main() {
    constexpr std::size_t count = 500000;
    constexpr int frames = 60;
    constexpr float dt = 0.016f;
    constexpr float max_bound = 15.0f;

    soa_vs_aos::AosParticles aos(count);
    soa_vs_aos::SoaParticles soa(count);

    // Warmup
    volatile auto w1 = aos.integrate_and_cull(dt, max_bound, 2);
    volatile auto w2 = soa.integrate_and_cull(dt, max_bound, 2);
    (void)w1;
    (void)w2;

    auto t0 = std::chrono::high_resolution_clock::now();
    volatile soa_vs_aos::CullResult r_aos = aos.integrate_and_cull(dt, max_bound, frames);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto t2 = std::chrono::high_resolution_clock::now();
    volatile soa_vs_aos::CullResult r_soa = soa.integrate_and_cull(dt, max_bound, frames);
    auto t3 = std::chrono::high_resolution_clock::now();

    double aos_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double soa_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();
    (void)r_aos;
    (void)r_soa;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "SoaVsAos (" << count << " particles, " << frames << " frames):\n";
    std::cout << "AoS (128B struct): " << aos_ms << " ms\n";
    std::cout << "SoA (hot arrays): " << soa_ms << " ms (" << (aos_ms / soa_ms) << "x faster)\n";
    return 0;
}
