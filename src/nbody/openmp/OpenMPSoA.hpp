#ifndef NBODY_OPENMP_SOA_HPP
#define NBODY_OPENMP_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/Physics.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class OpenMPSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations;

public:
    explicit OpenMPSoA(const SimulationConfig& config);
    ~OpenMPSoA() override = default;

    void computeOneIteration() override;
    const BodiesSoA& bodies() const;

protected:
    void initIteration();
    void computeBodiesAcceleration();
};


#endif
