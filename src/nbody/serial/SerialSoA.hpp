#ifndef NBODY_SERIAL_SOA_HPP
#define NBODY_SERIAL_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class SerialSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations;

public:
    explicit SerialSoA(const SimulationConfig& config);
    ~SerialSoA() override = default;

    void computeOneIteration() override;
    const BodiesSoA& bodies() const;

protected:
    void initIteration();
    void computeBodiesAcceleration();
};


#endif
