#ifndef NBODY_COMMON_CORE_SIMULATION_CONFIG_HPP
#define NBODY_COMMON_CORE_SIMULATION_CONFIG_HPP

#include <cstddef>
#include <string>

enum class Implementation {
    SerialAoS,
    SerialSoA,
    SIMD,
    OpenMP,
    CUDA
};

struct SimulationConfig {
    Implementation implementation{Implementation::SerialAoS};
    std::size_t num_bodies{2};
    std::size_t num_steps{100};
    std::string scheme{"galaxy"};
    double dt{0.001};
    double gravitational_constant{1.0};
    double softening{1.0e-5};
    std::string input_path{};
    std::string output_path{"trajectory.csv"};
    bool write_trajectory{true};
};

[[nodiscard]] Implementation implementation_from_string(const std::string& value);
[[nodiscard]] std::string to_string(Implementation implementation);
void validate_config(const SimulationConfig& config);


#endif
