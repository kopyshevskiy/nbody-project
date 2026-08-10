#include "common/utils/ArgumentsReader.hpp"
#include "common/utils/Timer.hpp"
#include "nbody/openmp/OpenMPSoA.hpp"
#include "nbody/serial/SerialAoS.hpp"
#include "nbody/serial/SerialSoA.hpp"
#include "nbody/simd/SIMDSoA.hpp"

#if defined(NBODY_ENABLE_CUDA)
#include "nbody/cuda/CudaSoA.hpp"
#endif

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

Implementation ImplTag = Implementation::SerialAoS;
std::size_t NBodies = 2;
std::size_t NIterations = 100;
std::string Scheme = "galaxy";
double Dt = 0.001;
double GravitationalConstant = 1.0;
double Softening = 1.0e-5;
std::string InputPath;
std::string OutputPath = "trajectory.csv";
bool WriteTrajectory = true;

void setGlobalOptions(const SimulationConfig& config) {
    ImplTag = config.implementation;
    NBodies = config.num_bodies;
    NIterations = config.num_steps;
    Scheme = config.scheme;
    Dt = config.dt;
    GravitationalConstant = config.gravitational_constant;
    Softening = config.softening;
    InputPath = config.input_path;
    OutputPath = config.output_path;
    WriteTrajectory = config.write_trajectory;
}

SimulationConfig makeConfig() {
    SimulationConfig config;
    config.implementation = ImplTag;
    config.num_bodies = NBodies;
    config.num_steps = NIterations;
    config.scheme = Scheme;
    config.dt = Dt;
    config.gravitational_constant = GravitationalConstant;
    config.softening = Softening;
    config.input_path = InputPath;
    config.output_path = OutputPath;
    config.write_trajectory = WriteTrajectory;
    return config;
}

SimulationNBodyInterface* createImplem() {
    const SimulationConfig config = makeConfig();
    switch (ImplTag) {
        case Implementation::SerialAoS:
            return new SerialAoS(config);
        case Implementation::SerialSoA:
            return new SerialSoA(config);
        case Implementation::SIMD:
            return new SIMDSoA(config);
        case Implementation::OpenMP:
            return new OpenMPSoA(config);
        case Implementation::CUDA:
#if defined(NBODY_ENABLE_CUDA)
            return new CudaSoA(config);
#else
            throw std::runtime_error(
                "CUDA support is not enabled in this build");
#endif
        default:
            throw std::runtime_error("selected implementation is not available yet");
    }
}

int main(int argc, char** argv) {
    try {
        setGlobalOptions(parse_arguments(argc, argv));
        Timer timer;

        SimulationNBodyInterface* simulation = createImplem();

        timer.start();
        simulation->run();
        const double elapsed = timer.elapsed_seconds();

        std::cout << "implementation=" << to_string(ImplTag)
                  << " bodies=" << simulation->size()
                  << " steps=" << NIterations
                  << " time_seconds=" << elapsed
                  << '\n';
        delete simulation;
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
