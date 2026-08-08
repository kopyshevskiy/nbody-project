#ifndef NBODY_SERIAL_AOS_HPP
#define NBODY_SERIAL_AOS_HPP

#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

#include <vector>

class SerialAoS : public SimulationNBodyInterface {
protected:
    std::vector<AccelerationAoS> accelerations;

public:
    explicit SerialAoS(const SimulationConfig& config);
    ~SerialAoS() override = default;

    void computeOneIteration() override;
    const BodiesAoS& bodies() const;

protected:
    void initIteration();
    void computeBodiesAcceleration();
};


#endif
