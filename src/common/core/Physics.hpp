#ifndef NBODY_COMMON_CORE_PHYSICS_HPP
#define NBODY_COMMON_CORE_PHYSICS_HPP

#include "common/core/Bodies.hpp"

#include <vector>

struct PhysicsParameters {
    double gravitational_constant{1.0};
    double softening{1.0e-5};
};

AccelerationAoS acceleration_from_body(
    const Body& target, const Body& source, const PhysicsParameters& params);
std::vector<AccelerationAoS> compute_accelerations_aos(
    const BodiesAoS& bodies, const PhysicsParameters& params);
AccelerationsSoA compute_accelerations_soa(
    const BodiesSoA& bodies, const PhysicsParameters& params);


#endif
