#ifndef NBODY_CUDA_SOA_HPP
#define NBODY_CUDA_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class CudaSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations; /*!< Structure of acceleration arrays. */

    double* d_qx{nullptr}; /*!< Device positions x. */
    double* d_qy{nullptr}; /*!< Device positions y. */
    double* d_qz{nullptr}; /*!< Device positions z. */
    double* d_m{nullptr};  /*!< Device masses. */
    double* d_ax{nullptr}; /*!< Device accelerations x. */
    double* d_ay{nullptr}; /*!< Device accelerations y. */
    double* d_az{nullptr}; /*!< Device accelerations z. */

public:
    /*!
     * \brief Constructor.
     */
    explicit CudaSoA(const SimulationConfig& config);

    /*!
     * \brief Destructor.
     */
    ~CudaSoA() override;

    /*!
     * \brief Compute one simulation iteration.
     */
    void computeOneIteration() override;

    /*!
     * \brief SoA bodies getter.
     */
    const BodiesSoA& bodies() const;

protected:
    /*!
     * \brief Initialize acceleration buffers.
     */
    void initIteration();

    /*!
     * \brief Compute bodies accelerations on the GPU.
     */
    void computeBodiesAcceleration();

    /*!
     * \brief Release CUDA device buffers.
     */
    void releaseCudaResources() noexcept;
};

#endif
