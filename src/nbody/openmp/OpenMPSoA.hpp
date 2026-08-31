#ifndef NBODY_OPENMP_SOA_HPP
#define NBODY_OPENMP_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/Physics.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class OpenMPSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations; /*!< Structure of acceleration arrays. */

public:
    /*!
     * \brief Constructor.
     */
    explicit OpenMPSoA(const SimulationConfig& config);

    /*!
     * \brief Destructor.
     */
    ~OpenMPSoA() override = default;

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
     * \brief Compute bodies accelerations with OpenMP.
     */
    void computeBodiesAcceleration();
};


#endif
