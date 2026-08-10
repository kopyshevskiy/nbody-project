#include "nbody/openmp/OpenMPSoA.hpp"

#include <cmath>
#include <vector>

OpenMPSoA::OpenMPSoA(const SimulationConfig& config)
    : SimulationNBodyInterface(config),
      accelerations{
          std::vector<double>(bodies_.getN()),
          std::vector<double>(bodies_.getN()),
          std::vector<double>(bodies_.getN())} {}

void OpenMPSoA::initIteration() {
    const std::size_t count = bodies_.getN();
    for (std::size_t iBody = 0; iBody < count; ++iBody) {
        accelerations.ax[iBody] = 0.0;
        accelerations.ay[iBody] = 0.0;
        accelerations.az[iBody] = 0.0;
    }
}

void OpenMPSoA::computeBodiesAcceleration() {
    const BodiesSoA& data = bodies_.getDataSoA();
    const std::size_t count = bodies_.getN();
    const double* mass = data.m.data();
    const double* x = data.qx.data();
    const double* y = data.qy.data();
    const double* z = data.qz.data();
    double* ax = accelerations.ax.data();
    double* ay = accelerations.ay.data();
    double* az = accelerations.az.data();
    const double softening_squared = physics_.softening * physics_.softening;
    const double gravitational_constant = physics_.gravitational_constant;

    // The schedule is selected externally for the scaling experiments.
#pragma omp parallel for schedule(runtime)
    for (std::size_t iBody = 0; iBody < count; ++iBody) {
        const double xi = x[iBody];
        const double yi = y[iBody];
        const double zi = z[iBody];
        double acceleration_x = 0.0;
        double acceleration_y = 0.0;
        double acceleration_z = 0.0;

        for (std::size_t jBody = 0; jBody < count; ++jBody) {
            const double dx = x[jBody] - xi;
            const double dy = y[jBody] - yi;
            const double dz = z[jBody] - zi;
            const double distance_squared =
                dx * dx + dy * dy + dz * dz + softening_squared;
            const double inverse_distance = 1.0 / std::sqrt(distance_squared);
            const double inverse_distance_cubed =
                inverse_distance * inverse_distance * inverse_distance;
            const double factor =
                gravitational_constant * mass[jBody] * inverse_distance_cubed;

            acceleration_x += dx * factor;
            acceleration_y += dy * factor;
            acceleration_z += dz * factor;
        }

        ax[iBody] = acceleration_x;
        ay[iBody] = acceleration_y;
        az[iBody] = acceleration_z;
    }
}

void OpenMPSoA::computeOneIteration() {
    initIteration();
    computeBodiesAcceleration();
    bodies_.updatePositionsAndVelocities(accelerations, config_.dt);
    time_ += config_.dt;
}

const BodiesSoA& OpenMPSoA::bodies() const {
    return bodies_.getDataSoA();
}
