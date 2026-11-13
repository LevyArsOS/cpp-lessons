#include "ContiguousObjects/include/particle_systems.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

int main() {
    using contiguous_objects::ContiguousParticleSystem;
    using contiguous_objects::PointerParticleSystem;

    // 1. 100,000 particles produce identical final checksums across both layouts
    constexpr std::size_t count = 100000;
    PointerParticleSystem ptr_sys(count, true);
    ContiguousParticleSystem val_sys(count);

    assert(ptr_sys.size() == count);
    assert(val_sys.size() == count);

    ptr_sys.step(0.016f, 10);
    val_sys.step(0.016f, 10);

    double diff = std::fabs(ptr_sys.checksum() - val_sys.checksum());
    assert(diff < 1e-2);

    // 2. Contiguous memory stride verification
    const auto* base = val_sys.data();
    for (std::size_t i = 0; i < 16; ++i) {
        assert(&val_sys.items()[i] == base + i);
    }

    // 3. Empty system edge case
    PointerParticleSystem empty_ptr(0);
    ContiguousParticleSystem empty_val(0);
    empty_ptr.step(0.016f, 5);
    empty_val.step(0.016f, 5);
    assert(empty_ptr.checksum() == 0.0);
    assert(empty_val.checksum() == 0.0);

    std::cout << "All ContiguousObjects tests passed successfully.\n";
    return 0;
}
