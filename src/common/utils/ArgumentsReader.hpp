#ifndef NBODY_COMMON_UTILS_ARGUMENTS_READER_HPP
#define NBODY_COMMON_UTILS_ARGUMENTS_READER_HPP

#include "common/core/SimulationConfig.hpp"

/*!
 * \brief Parse command line arguments.
 */
SimulationConfig parse_arguments(int argc, char** argv);

/*!
 * \brief Print command line usage.
 */
void print_usage(const char* program_name);


#endif
