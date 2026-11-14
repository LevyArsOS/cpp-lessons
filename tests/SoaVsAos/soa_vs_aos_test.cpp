#include "SoaVsAos/include/particle_layouts.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

int main() {
    using soa_vs_aos::AosParticles;
    using soa_vs_aos::CullResult;
    using soa_vs_aos::SoaParticles;

    // 1. 100,000 particles produce identical visible counts and checksums
    constexpr std::size_t count = 100000;
    AosParticles aos(count);
    SoaParticles soa(count);

    assert(aos.size() == count);
    assert(soa.size() == count);

    CullResult r_aos = aos.integrate_and_cull(0.016f, 15.0f, 10);
    CullResult r_soa = soa.integrate_and_cull(0.016f, 15.0f, 10);

    assert(r_aos.visible_count == r_soa.visible_count);
    assert(r_aos.visible_count > 0 && r_aos.visible_count < count);
    assert(std::fabs(r_aos.checksum - r_soa.checksum) < 1e-2);

    // 2. Edge cases: zero particles and tight culling bound (0 visible)
    AosParticles empty_aos(0);
    SoaParticles empty_soa(0);
    CullResult e_aos = empty_aos.integrate_and_cull(0.016f, 10.0f, 5);
    CullResult e_soa = empty_soa.integrate_and_cull(0.016f, 10.0f, 5);
    assert(e_aos.visible_count == 0 && e_soa.visible_count == 0);
    assert(e_aos.checksum == 0.0 && e_soa.checksum == 0.0);

    AosParticles tight_aos(100);
    SoaParticles tight_soa(100);
    CullResult t_aos = tight_aos.integrate_and_cull(0.016f, -1.0f, 1);
    CullResult t_soa = tight_soa.integrate_and_cull(0.016f, -1.0f, 1);
    assert(t_aos.visible_count == 0 && t_soa.visible_count == 0);

    std::cout << "All SoaVsAos tests passed successfully.\n";
    return 0;
}
