#ifndef NBODY_SERIAL_AOS_HPP
#define NBODY_SERIAL_AOS_HPP

#include "common/core/Bodies.hpp"
#include "common/core/SimulationConfig.hpp"
#include "common/core/SimulationNBodyInterface.hpp"

#include <vector>

class SerialAoS : public SimulationNBodyInterface {
protected:
    std::vector<AccelerationAoS> accelerations; /*!< Array of acceleration structures. */

public:
    /*!
     * \brief Constructor.
     */
    explicit SerialAoS(const SimulationConfig& config);

    /*!
     * \brief Destructor.
     */
    ~SerialAoS() override = default;

    /*!
     * \brief Compute one simulation iteration.
     */
    void computeOneIteration() override;

    /*!
     * \brief AoS bodies getter.
     */
    const BodiesAoS& bodies() const;

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
