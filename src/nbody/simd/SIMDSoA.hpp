#ifndef NBODY_SIMD_SOA_HPP
#define NBODY_SIMD_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/Physics.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class SIMDSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations;

public:
    explicit SIMDSoA(const SimulationConfig& config);
    ~SIMDSoA() override = default;

    void computeOneIteration() override;
    const BodiesSoA& bodies() const;

protected:
    void initIteration();
    void computeBodiesAcceleration();
};


#endif
