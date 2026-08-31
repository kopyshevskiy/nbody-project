#ifndef NBODY_COMMON_CORE_TRAJECTORY_WRITER_HPP
#define NBODY_COMMON_CORE_TRAJECTORY_WRITER_HPP

#include "common/core/Bodies.hpp"

#include <fstream>
#include <string>

class TrajectoryWriter {
public:
    /*!
     * \brief Constructor.
     */
    explicit TrajectoryWriter(const std::string& path);

    /*!
     * \brief Write one trajectory step from AoS storage.
     */
    void write_step(std::size_t step, double time, const BodiesAoS& bodies);

    /*!
     * \brief Write one trajectory step from SoA storage.
     */
    void write_step(std::size_t step, double time, const BodiesSoA& bodies);

private:
    std::ofstream output_; /*!< Output trajectory stream. */
};


#endif
