#include "common/core/SimulationNBodyInterface.hpp"

#include "common/core/InitialCondition.hpp"
#include "common/core/TrajectoryWriter.hpp"

BodiesAoS make_initial_conditions(const SimulationConfig& config) {
    if (!config.input_path.empty()) {
        return load_initial_conditions(config.input_path);
    }
    if (config.scheme == "galaxy") {
        return generate_galaxy(config.num_bodies, config.gravitational_constant);
    }
    if (config.scheme == "spiral") {
        return generate_spiral(config.num_bodies, config.gravitational_constant);
    }
    return generate_circular_orbit(config.num_bodies);
}

SimulationNBodyInterface::SimulationNBodyInterface(const SimulationConfig& config)
    : config_(config),
      physics_{config.gravitational_constant, config.softening},
      bodies_(make_initial_conditions(config)) {
    validate_config(config_);
    dt_ = config_.dt;
}

void SimulationNBodyInterface::run() {
    TrajectoryWriter* writer = nullptr;
    if (config_.write_trajectory) {
        writer = new TrajectoryWriter(config_.output_path);
        writer->write_step(0, time_, bodies_.getDataAoS());
    }

    for (std::size_t step_index = 1; step_index <= config_.num_steps; ++step_index) {
        computeOneIteration();
        if (writer) {
            writer->write_step(step_index, time_, bodies_.getDataAoS());
        }
    }

    delete writer;
}

std::size_t SimulationNBodyInterface::size() const {
    return bodies_.getN();
}

const Bodies<double>& SimulationNBodyInterface::getBodies() const {
    return bodies_;
}

double SimulationNBodyInterface::getDt() const {
    return dt_;
}
