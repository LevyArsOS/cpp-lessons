#include "ContiguousObjects/include/particle_systems.hpp"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>

int main() {
    constexpr std::size_t count = 100000;
    constexpr int frames = 60;
    constexpr float dt = 0.016f;

    contiguous_objects::PointerParticleSystem ptr_sys(count, true);
    contiguous_objects::ContiguousParticleSystem val_sys(count);

    auto t0 = std::chrono::high_resolution_clock::now();
    ptr_sys.step(dt, frames);
    volatile double ptr_sum = ptr_sys.checksum();
    auto t1 = std::chrono::high_resolution_clock::now();

    auto t2 = std::chrono::high_resolution_clock::now();
    val_sys.step(dt, frames);
    volatile double val_sum = val_sys.checksum();
    auto t3 = std::chrono::high_resolution_clock::now();

    double ptr_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double val_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();
    (void)ptr_sum;
    (void)val_sum;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "ContiguousObjects (" << count << " particles, " << frames << " frames):\n";
    std::cout << "std::vector<unique_ptr<Particle>>: " << ptr_ms << " ms\n";
    std::cout << "std::vector<Particle> (value):     " << val_ms << " ms (" << (ptr_ms / val_ms) << "x faster)\n";
    return 0;
}
