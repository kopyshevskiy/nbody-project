#include "nbody/serial/SerialSoA.hpp"

#include <cmath>

SerialSoA::SerialSoA(const SimulationConfig& config)
    : SimulationNBodyInterface(config) {
    accelerations.ax.resize(bodies_.getN());
    accelerations.ay.resize(bodies_.getN());
    accelerations.az.resize(bodies_.getN());
}

void SerialSoA::initIteration() {
    const std::size_t count = bodies_.getN();
    for (std::size_t iBody = 0; iBody < count; ++iBody) {
        accelerations.ax[iBody] = 0.0;
        accelerations.ay[iBody] = 0.0;
        accelerations.az[iBody] = 0.0;
    }
}

void SerialSoA::computeBodiesAcceleration() {
    const BodiesSoA& data = bodies_.getDataSoA();
    const std::size_t count = bodies_.getN();
    const double softSquared = physics_.softening * physics_.softening;
    const double gravitationalConstant = physics_.gravitational_constant;

    const double* qx = data.qx.data();
    const double* qy = data.qy.data();
    const double* qz = data.qz.data();
    const double* mass = data.m.data();

    for (std::size_t iBody = 0; iBody < count; ++iBody) {
        double aix = 0.0;
        double aiy = 0.0;
        double aiz = 0.0;

        const double qix = qx[iBody];
        const double qiy = qy[iBody];
        const double qiz = qz[iBody];

        for (std::size_t jBody = 0; jBody < count; ++jBody) {
            const double rijx = qx[jBody] - qix;
            const double rijy = qy[jBody] - qiy;
            const double rijz = qz[jBody] - qiz;
            const double rijSquared =
                rijx * rijx + rijy * rijy + rijz * rijz;
            const double distanceSquared = rijSquared + softSquared;
            const double inverseDistance = 1.0 / std::sqrt(distanceSquared);
            const double inverseDistanceCubed =
                inverseDistance * inverseDistance * inverseDistance;
            const double acceleration =
                gravitationalConstant * mass[jBody] * inverseDistanceCubed;

            aix += acceleration * rijx;
            aiy += acceleration * rijy;
            aiz += acceleration * rijz;
        }

        accelerations.ax[iBody] = aix;
        accelerations.ay[iBody] = aiy;
        accelerations.az[iBody] = aiz;
    }
}

void SerialSoA::computeOneIteration() {
    initIteration();
    computeBodiesAcceleration();
    bodies_.updatePositionsAndVelocities(accelerations, config_.dt);
    time_ += config_.dt;
}

const BodiesSoA& SerialSoA::bodies() const {
    return bodies_.getDataSoA();
}
