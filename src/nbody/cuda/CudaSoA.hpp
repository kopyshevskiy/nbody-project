#ifndef NBODY_CUDA_SOA_HPP
#define NBODY_CUDA_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class CudaSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations;

    double* d_qx{nullptr};
    double* d_qy{nullptr};
    double* d_qz{nullptr};
    double* d_m{nullptr};
    double* d_ax{nullptr};
    double* d_ay{nullptr};
    double* d_az{nullptr};

public:
    explicit CudaSoA(const SimulationConfig& config);
    ~CudaSoA() override;

    void computeOneIteration() override;
    const BodiesSoA& bodies() const;

protected:
    void initIteration();
    void computeBodiesAcceleration();
    void releaseCudaResources() noexcept;
};

#endif
