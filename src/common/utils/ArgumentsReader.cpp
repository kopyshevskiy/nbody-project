#include "common/utils/ArgumentsReader.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

std::string require_value(int& index, int argc, char** argv, const std::string& option) {
    if (index + 1 >= argc) {
        throw std::invalid_argument("missing value for " + option);
    }
    ++index;
    return argv[index];
}

SimulationConfig parse_arguments(int argc, char** argv) {
    SimulationConfig config;

    for (int i = 1; i < argc; ++i) {
        const std::string option = argv[i];
        if (option == "--help" || option == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else if (option == "--implementation") {
            config.implementation =
                implementation_from_string(require_value(i, argc, argv, option));
        } else if (option == "--bodies") {
            config.num_bodies = static_cast<std::size_t>(std::stoull(require_value(i, argc, argv, option)));
        } else if (option == "--steps") {
            config.num_steps = static_cast<std::size_t>(std::stoull(require_value(i, argc, argv, option)));
        } else if (option == "--scheme") {
            config.scheme = require_value(i, argc, argv, option);
        } else if (option == "--dt") {
            config.dt = std::stod(require_value(i, argc, argv, option));
        } else if (option == "--g") {
            config.gravitational_constant = std::stod(require_value(i, argc, argv, option));
        } else if (option == "--softening") {
            config.softening = std::stod(require_value(i, argc, argv, option));
        } else if (option == "--input") {
            config.input_path = require_value(i, argc, argv, option);
        } else if (option == "--output") {
            config.output_path = require_value(i, argc, argv, option);
        } else if (option == "--no-output") {
            config.write_trajectory = false;
        } else {
            throw std::invalid_argument("unknown option: " + option);
        }
    }

    validate_config(config);
    return config;
}

void print_usage(const char* program_name) {
    std::cout
        << "Usage: " << program_name << " [options]\n"
        << "  --implementation serial-aos|serial-soa|simd|openmp|cuda\n"
        << "  --input PATH\n"
        << "  --output PATH\n"
        << "  --bodies N\n"
        << "  --steps N\n"
        << "  --scheme galaxy|spiral|circular\n"
        << "  --dt VALUE\n"
        << "  --g VALUE\n"
        << "  --softening VALUE\n"
        << "  --no-output\n";
}
