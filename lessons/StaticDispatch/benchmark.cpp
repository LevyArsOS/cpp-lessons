#include "StaticDispatch/include/dispatch_models.hpp"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>

template <typename World>
static double measure_world(std::size_t count, float dt, int frames) {
    World world(count);
    world.update_all(dt, 2);  // warmup
    auto t0 = std::chrono::high_resolution_clock::now();
    world.update_all(dt, frames);
    volatile double sum = world.checksum();
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sum;
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

int main() {
    constexpr std::size_t count = 50000;
    constexpr int frames = 60;
    constexpr float dt = 0.016f;

    double v_ms = measure_world<static_dispatch::VirtualWorld>(count, dt, frames);
    double f_ms = measure_world<static_dispatch::FnTableWorld>(count, dt, frames);
    double t_ms = measure_world<static_dispatch::TagSwitchWorld>(count, dt, frames);
    double c_ms = measure_world<static_dispatch::CrtpWorld>(count, dt, frames);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "StaticDispatch (" << count << " entities, " << frames << " frames, 8 behavior tags):\n";
    std::cout << "Virtual dispatch:   " << v_ms << " ms\n";
    std::cout << "Fn pointer table:   " << f_ms << " ms (" << (v_ms / f_ms) << "x faster)\n";
    std::cout << "Tag switch:         " << t_ms << " ms (" << (v_ms / t_ms) << "x faster)\n";
    std::cout << "CRTP buckets:       " << c_ms << " ms (" << (v_ms / c_ms) << "x faster)\n";
    return 0;
}
