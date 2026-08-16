#include "nbody/serial/SerialAoS.hpp"

#include <cmath>

SerialAoS::SerialAoS(const SimulationConfig& config)
    : SimulationNBodyInterface(config) {
    accelerations.resize(bodies_.getN());
}

void SerialAoS::initIteration() {
    for (std::size_t iBody = 0; iBody < bodies_.getN(); ++iBody) {
        accelerations[iBody].ax = 0.0;
        accelerations[iBody].ay = 0.0;
        accelerations[iBody].az = 0.0;
    }
}

void SerialAoS::computeBodiesAcceleration() {
    const BodiesAoS& data = bodies_.getDataAoS();
    const double softSquared = physics_.softening * physics_.softening;

    for (std::size_t iBody = 0; iBody < bodies_.getN(); ++iBody) {
        for (std::size_t jBody = 0; jBody < bodies_.getN(); ++jBody) {
            const double rijx = data[jBody].qx - data[iBody].qx;
            const double rijy = data[jBody].qy - data[iBody].qy;
            const double rijz = data[jBody].qz - data[iBody].qz;
            const double rijSquared =
                rijx * rijx + rijy * rijy + rijz * rijz;
            const double inverseDistance =
                1.0 / std::sqrt(rijSquared + softSquared);
            const double inverseDistanceCubed =
                inverseDistance * inverseDistance * inverseDistance;
            const double acceleration = physics_.gravitational_constant
                * data[jBody].m * inverseDistanceCubed;

            accelerations[iBody].ax += acceleration * rijx;
            accelerations[iBody].ay += acceleration * rijy;
            accelerations[iBody].az += acceleration * rijz;
        }
    }
}

void SerialAoS::computeOneIteration() {
    initIteration();
    computeBodiesAcceleration();
    bodies_.updatePositionsAndVelocities(accelerations, config_.dt);
    time_ += config_.dt;
}

const BodiesAoS& SerialAoS::bodies() const {
    return bodies_.getDataAoS();
}
