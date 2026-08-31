#ifndef NBODY_SERIAL_SOA_HPP
#define NBODY_SERIAL_SOA_HPP

#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

class SerialSoA : public SimulationNBodyInterface {
protected:
    AccelerationsSoA accelerations; /*!< Structure of acceleration arrays. */

public:
    /*!
     * \brief Constructor.
     */
    explicit SerialSoA(const SimulationConfig& config);

    /*!
     * \brief Destructor.
     */
    ~SerialSoA() override = default;

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
     * \brief Compute bodies accelerations.
     */
    void computeBodiesAcceleration();
};


#endif
