#ifndef NBODY_COMMON_CORE_TRAJECTORY_READER_HPP
#define NBODY_COMMON_CORE_TRAJECTORY_READER_HPP

#include "common/core/Bodies.hpp"

#include <fstream>
#include <string>

class TrajectoryReader {
public:
    /*!
     * \brief Constructor.
     */
    explicit TrajectoryReader(const std::string& path);

    /*!
     * \brief Read the next trajectory frame.
     */
    bool read_next_frame(BodiesAoS& bodies, std::size_t& step, double& time);

private:
    std::ifstream input_;             /*!< Input trajectory stream. */
    std::string pending_line_;        /*!< Pending line between frames. */
    bool has_pending_line_{false};    /*!< Pending line flag. */
};


#endif
