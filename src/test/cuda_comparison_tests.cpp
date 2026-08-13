#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "nbody/cuda/CudaSoA.hpp"
#include "nbody/serial/SerialSoA.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

bool almost_equal(double lhs, double rhs) {
    constexpr double tolerance = 1.0e-9;
    const double scale = std::max(std::abs(lhs), std::abs(rhs));
    return std::abs(lhs - rhs) <= tolerance * (1.0 + scale);
}

void require(bool condition) {
    if (!condition) {
        throw std::runtime_error("serial/CUDA comparison failed");
    }
}

int main() {
    SimulationConfig config;
    config.num_bodies = 32;
    config.num_steps = 5;
    config.dt = 0.001;
    config.write_trajectory = false;

    SerialSoA serial(config);
    CudaSoA cuda(config);
    serial.run();
    cuda.run();

    const BodiesAoS serial_result = to_aos(serial.bodies());
    const BodiesAoS cuda_result = to_aos(cuda.bodies());
    require(serial_result.size() == cuda_result.size());

    for (std::size_t i = 0; i < serial_result.size(); ++i) {
        require(almost_equal(serial_result[i].m, cuda_result[i].m));
        require(almost_equal(serial_result[i].qx, cuda_result[i].qx));
        require(almost_equal(serial_result[i].qy, cuda_result[i].qy));
        require(almost_equal(serial_result[i].qz, cuda_result[i].qz));
        require(almost_equal(serial_result[i].vx, cuda_result[i].vx));
        require(almost_equal(serial_result[i].vy, cuda_result[i].vy));
        require(almost_equal(serial_result[i].vz, cuda_result[i].vz));
    }

    return 0;
}
