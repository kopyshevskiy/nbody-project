#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "nbody/serial/SerialAoS.hpp"
#include "nbody/serial/SerialSoA.hpp"

#include <cmath>
#include <stdexcept>

bool almost_equal(double lhs, double rhs) {
    constexpr double tolerance = 1.0e-12;
    return std::abs(lhs - rhs) < tolerance;
}

void require(bool condition) {
    if (!condition) {
        throw std::runtime_error("AoS/SoA comparison failed");
    }
}

int main() {
    SimulationConfig config;
    config.num_bodies = 8;
    config.num_steps = 20;
    config.dt = 0.001;
    config.write_trajectory = false;

    SerialAoS aos(config);
    SerialSoA soa(config);
    aos.run();
    soa.run();

    const BodiesAoS soa_result = to_aos(soa.bodies());
    require(aos.bodies().size() == soa_result.size());

    for (std::size_t i = 0; i < aos.bodies().size(); ++i) {
        const Body& aos_body = aos.bodies()[i];
        const Body& soa_body = soa_result[i];

        require(almost_equal(aos_body.m, soa_body.m));
        require(almost_equal(aos_body.qx, soa_body.qx));
        require(almost_equal(aos_body.qy, soa_body.qy));
        require(almost_equal(aos_body.qz, soa_body.qz));
        require(almost_equal(aos_body.vx, soa_body.vx));
        require(almost_equal(aos_body.vy, soa_body.vy));
        require(almost_equal(aos_body.vz, soa_body.vz));
    }

    return 0;
}
