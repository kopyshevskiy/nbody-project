#ifndef NBODY_COMMON_UTILS_ARGUMENTS_READER_HPP
#define NBODY_COMMON_UTILS_ARGUMENTS_READER_HPP

#include "common/core/SimulationConfig.hpp"

[[nodiscard]] SimulationConfig parse_arguments(int argc, char** argv);
void print_usage(const char* program_name);


#endif
