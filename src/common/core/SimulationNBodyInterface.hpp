#ifndef NBODY_COMMON_CORE_SIMULATION_N_BODY_INTERFACE_HPP
#define NBODY_COMMON_CORE_SIMULATION_N_BODY_INTERFACE_HPP

#include "common/core/Bodies.hpp"
#include "common/core/Physics.hpp"
#include "common/core/SimulationConfig.hpp"

#include <cstddef>

class SimulationNBodyInterface {
public:
    /*!
     * \brief Constructor.
     */
    explicit SimulationNBodyInterface(const SimulationConfig& config);

    /*!
     * \brief Destructor.
     */
    virtual ~SimulationNBodyInterface() = default;

    /*!
     * \brief Compute one simulation iteration.
     */
    virtual void computeOneIteration() = 0;

    /*!
     * \brief Run the complete simulation.
     */
    virtual void run();

    /*!
     * \brief Number of bodies getter.
     */
    std::size_t size() const;

    /*!
     * \brief Bodies getter.
     */
    const Bodies<double>& getBodies() const;

    /*!
     * \brief Time step getter.
     */
    double getDt() const;

protected:
    SimulationConfig config_;   /*!< Simulation configuration. */
    PhysicsParameters physics_; /*!< Physical parameters. */
    Bodies<double> bodies_;     /*!< Bodies object. */
    double dt_{0.0};            /*!< Time step value. */
    double time_{0.0};          /*!< Current simulation time. */
};


#endif
