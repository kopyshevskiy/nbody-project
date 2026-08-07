#ifndef NBODY_COMMON_CORE_SIMULATION_N_BODY_INTERFACE_HPP
#define NBODY_COMMON_CORE_SIMULATION_N_BODY_INTERFACE_HPP

#include "common/core/Bodies.hpp"
#include "common/core/Physics.hpp"
#include "common/core/SimulationConfig.hpp"

#include <cstddef>

class SimulationNBodyInterface {
public:
    explicit SimulationNBodyInterface(const SimulationConfig& config);
    virtual ~SimulationNBodyInterface() = default;

    virtual void computeOneIteration() = 0;
    virtual void run();

    std::size_t size() const;
    const Bodies<double>& getBodies() const;
    double getDt() const;

protected:
    SimulationConfig config_;
    PhysicsParameters physics_;
    Bodies<double> bodies_;
    double dt_{0.0};
    double time_{0.0};
};


#endif
