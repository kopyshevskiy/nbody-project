#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "nbody/serial/SerialSoA.hpp"
#include "nbody/simd/SIMDSoA.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

bool almost_equal(double lhs, double rhs) {
    constexpr double tolerance = 1.0e-10;
    const double scale = std::max(std::abs(lhs), std::abs(rhs));
    return std::abs(lhs - rhs) <= tolerance * (1.0 + scale);
}

void require(bool condition) {
    if (!condition) {
        throw std::runtime_error("serial/SIMD comparison failed");
    }
}

int main() {
    SimulationConfig config;
    config.num_bodies = 12;
    config.num_steps = 20;
    config.dt = 0.001;
    config.write_trajectory = false;

    SerialSoA serial(config);
    SIMDSoA simd(config);
    serial.run();
    simd.run();

    const BodiesAoS serial_result = to_aos(serial.bodies());
    const BodiesAoS simd_result = to_aos(simd.bodies());
    require(serial_result.size() == simd_result.size());

    for (std::size_t i = 0; i < serial_result.size(); ++i) {
        require(almost_equal(serial_result[i].m, simd_result[i].m));
        require(almost_equal(serial_result[i].qx, simd_result[i].qx));
        require(almost_equal(serial_result[i].qy, simd_result[i].qy));
        require(almost_equal(serial_result[i].qz, simd_result[i].qz));
        require(almost_equal(serial_result[i].vx, simd_result[i].vx));
        require(almost_equal(serial_result[i].vy, simd_result[i].vy));
        require(almost_equal(serial_result[i].vz, simd_result[i].vz));
    }

    config.num_bodies = 13;
    bool rejected = false;
    try {
        SIMDSoA invalid_simd(config);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected);

    return 0;
}
