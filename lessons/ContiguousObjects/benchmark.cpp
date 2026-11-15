#include "ContiguousObjects/include/particle_systems.hpp"
#include "common/bench_utils.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>

int main() {
    constexpr std::size_t count = 100000;
    constexpr int frames = 60;
    constexpr float dt = 0.016f;

    contiguous_objects::PointerParticleSystem ptr_sys(count, true);
    contiguous_objects::ContiguousParticleSystem val_sys(count);

    double ptr_ms = bench::measure_ms([&]() {
        ptr_sys.step(dt, frames);
        bench::do_not_optimize(ptr_sys.checksum());
    });

    double val_ms = bench::measure_ms([&]() {
        val_sys.step(dt, frames);
        bench::do_not_optimize(val_sys.checksum());
    });

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "ContiguousObjects (" << count << " particles, " << frames << " frames):\n";
    std::cout << "std::vector<unique_ptr<Particle>>: " << ptr_ms << " ms\n";
    std::cout << "std::vector<Particle> (value):     " << val_ms << " ms (" << (ptr_ms / val_ms) << "x faster)\n";
    return 0;
}
