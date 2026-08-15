#include "common/core/SimulationConfig.hpp"

#include <cmath>
#include <stdexcept>

Implementation implementation_from_string(const std::string& value) {
    if (value == "serial-aos") {
        return Implementation::SerialAoS;
    }
    if (value == "serial-soa") {
        return Implementation::SerialSoA;
    }
    if (value == "simd") {
        return Implementation::SIMD;
    }
    if (value == "openmp") {
        return Implementation::OpenMP;
    }
    if (value == "cuda") {
        return Implementation::CUDA;
    }
    throw std::invalid_argument("unknown implementation: " + value);
}

std::string to_string(Implementation implementation) {
    switch (implementation) {
        case Implementation::SerialAoS:
            return "serial-aos";
        case Implementation::SerialSoA:
            return "serial-soa";
        case Implementation::SIMD:
            return "simd";
        case Implementation::OpenMP:
            return "openmp";
        case Implementation::CUDA:
            return "cuda";
    }
    return "unknown";
}

void validate_config(const SimulationConfig& config) {
    if (config.scheme != "galaxy" && config.scheme != "circular") {
        throw std::invalid_argument("scheme must be galaxy or circular");
    }
    if (config.input_path.empty() && config.num_bodies == 0) {
        throw std::invalid_argument("the number of bodies must be greater than zero");
    }
    if (!std::isfinite(config.dt) || config.dt <= 0.0) {
        throw std::invalid_argument("the time step must be finite and greater than zero");
    }
    if (!std::isfinite(config.gravitational_constant)
        || config.gravitational_constant <= 0.0) {
        throw std::invalid_argument(
            "the gravitational constant must be finite and greater than zero");
    }
    if (!std::isfinite(config.softening) || config.softening <= 0.0) {
        throw std::invalid_argument("softening must be finite and greater than zero");
    }
    if (config.write_trajectory && config.output_path.empty()) {
        throw std::invalid_argument("the output path cannot be empty");
    }
}
