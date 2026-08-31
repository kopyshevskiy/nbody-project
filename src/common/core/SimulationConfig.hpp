#ifndef NBODY_COMMON_CORE_SIMULATION_CONFIG_HPP
#define NBODY_COMMON_CORE_SIMULATION_CONFIG_HPP

#include <cstddef>
#include <string>

enum class Implementation {
    SerialAoS, /*!< Sequential AoS implementation. */
    SerialSoA, /*!< Sequential SoA implementation. */
    SIMD,      /*!< SIMD SoA implementation. */
    OpenMP,    /*!< OpenMP SoA implementation. */
    CUDA       /*!< CUDA SoA implementation. */
};

/*!
 * \brief Simulation parameters parsed from command line.
 */
struct SimulationConfig {
    Implementation implementation{Implementation::SerialAoS}; /*!< Selected implementation. */
    std::size_t num_bodies{2};                                /*!< Number of bodies. */
    std::size_t num_steps{100};                               /*!< Number of time steps. */
    std::string scheme{"galaxy"};                             /*!< Initial condition scheme. */
    double dt{0.001};                                         /*!< Time step value. */
    double gravitational_constant{1.0};                       /*!< Gravitational constant. */
    double softening{1.0e-5};                                 /*!< Softening factor. */
    std::string input_path{};                                 /*!< Optional input path. */
    std::string output_path{"trajectory.csv"};                /*!< Trajectory output path. */
    bool write_trajectory{true};                              /*!< Trajectory writing flag. */
};

/*!
 * \brief Convert a string to an implementation tag.
 */
Implementation implementation_from_string(const std::string& value);

/*!
 * \brief Convert an implementation tag to string.
 */
std::string to_string(Implementation implementation);

/*!
 * \brief Validate simulation parameters.
 */
void validate_config(const SimulationConfig& config);


#endif
