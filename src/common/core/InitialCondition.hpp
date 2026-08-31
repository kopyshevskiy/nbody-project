#ifndef NBODY_COMMON_CORE_INITIAL_CONDITION_HPP
#define NBODY_COMMON_CORE_INITIAL_CONDITION_HPP

#include "common/core/Bodies.hpp"

#include <cstddef>
#include <string>

/*!
 * \brief Load bodies from an input file.
 */
BodiesAoS load_initial_conditions(const std::string& path);

/*!
 * \brief Generate bodies on a circular orbit.
 */
BodiesAoS generate_circular_orbit(std::size_t count);

/*!
 * \brief Generate a disk galaxy initial condition.
 */
BodiesAoS generate_galaxy(std::size_t count, double gravitational_constant);

/*!
 * \brief Generate a spiral galaxy initial condition.
 */
BodiesAoS generate_spiral(std::size_t count, double gravitational_constant);


#endif
