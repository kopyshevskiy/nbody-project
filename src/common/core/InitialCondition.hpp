#ifndef NBODY_COMMON_CORE_INITIAL_CONDITION_HPP
#define NBODY_COMMON_CORE_INITIAL_CONDITION_HPP

#include "common/core/Bodies.hpp"

#include <cstddef>
#include <string>

[[nodiscard]] BodiesAoS load_initial_conditions(const std::string& path);
[[nodiscard]] BodiesAoS generate_circular_orbit(std::size_t count);
[[nodiscard]] BodiesAoS generate_galaxy(std::size_t count, double gravitational_constant);
[[nodiscard]] BodiesAoS generate_spiral(std::size_t count, double gravitational_constant);


#endif
