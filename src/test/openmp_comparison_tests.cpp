#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "nbody/openmp/OpenMPSoA.hpp"
#include "nbody/serial/SerialSoA.hpp"

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
        throw std::runtime_error("serial/OpenMP comparison failed");
    }
}

int main() {
    SimulationConfig config;
    config.num_bodies = 13;
    config.num_steps = 20;
    config.dt = 0.001;
    config.write_trajectory = false;

    SerialSoA serial(config);
    OpenMPSoA openmp(config);
    serial.run();
    openmp.run();

    const BodiesAoS serial_result = to_aos(serial.bodies());
    const BodiesAoS openmp_result = to_aos(openmp.bodies());
    require(serial_result.size() == openmp_result.size());

    for (std::size_t i = 0; i < serial_result.size(); ++i) {
        require(almost_equal(serial_result[i].m, openmp_result[i].m));
        require(almost_equal(serial_result[i].qx, openmp_result[i].qx));
        require(almost_equal(serial_result[i].qy, openmp_result[i].qy));
        require(almost_equal(serial_result[i].qz, openmp_result[i].qz));
        require(almost_equal(serial_result[i].vx, openmp_result[i].vx));
        require(almost_equal(serial_result[i].vy, openmp_result[i].vy));
        require(almost_equal(serial_result[i].vz, openmp_result[i].vz));
    }

    return 0;
}
