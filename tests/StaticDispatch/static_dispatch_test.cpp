#include "StaticDispatch/include/dispatch_models.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

int main() {
    using static_dispatch::CrtpWorld;
    using static_dispatch::FnTableWorld;
    using static_dispatch::TagSwitchWorld;
    using static_dispatch::VirtualWorld;

    constexpr std::size_t count = 50000;
    constexpr int frames = 10;
    constexpr float dt = 0.016f;

    VirtualWorld v_world(count);
    FnTableWorld f_world(count);
    TagSwitchWorld t_world(count);
    CrtpWorld c_world(count);

    v_world.update_all(dt, frames);
    f_world.update_all(dt, frames);
    t_world.update_all(dt, frames);
    c_world.update_all(dt, frames);

    double cv = v_world.checksum();
    double cf = f_world.checksum();
    double ct = t_world.checksum();
    double cc = c_world.checksum();

    assert(std::fabs(cv - cf) < 1e-1);
    assert(std::fabs(cv - ct) < 1e-1);
    assert(std::fabs(cv - cc) < 1e-1);

    // Empty world safety
    VirtualWorld ev(0);
    FnTableWorld ef(0);
    TagSwitchWorld et(0);
    CrtpWorld ec(0);
    ev.update_all(dt, 5);
    ef.update_all(dt, 5);
    et.update_all(dt, 5);
    ec.update_all(dt, 5);
    assert(ev.checksum() == 0.0 && ef.checksum() == 0.0 && et.checksum() == 0.0 && ec.checksum() == 0.0);

    std::cout << "All StaticDispatch tests passed successfully.\n";
    return 0;
}
