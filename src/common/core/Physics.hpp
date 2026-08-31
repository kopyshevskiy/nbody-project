#ifndef NBODY_COMMON_CORE_PHYSICS_HPP
#define NBODY_COMMON_CORE_PHYSICS_HPP

#include "common/core/Bodies.hpp"

#include <vector>

/*!
 * \brief Physical constants used by the force computation.
 */
struct PhysicsParameters {
    double gravitational_constant{1.0}; /*!< Gravitational constant. */
    double softening{1.0e-5};           /*!< Softening factor. */
};

/*!
 * \brief Compute the acceleration contribution from source to target.
 */
AccelerationAoS acceleration_from_body(
    const Body& target, const Body& source, const PhysicsParameters& params);

/*!
 * \brief Compute all accelerations using AoS storage.
 */
std::vector<AccelerationAoS> compute_accelerations_aos(
    const BodiesAoS& bodies, const PhysicsParameters& params);

/*!
 * \brief Compute all accelerations using SoA storage.
 */
AccelerationsSoA compute_accelerations_soa(
    const BodiesSoA& bodies, const PhysicsParameters& params);


#endif
