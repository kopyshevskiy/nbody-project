#ifndef NBODY_COMMON_CORE_TRAJECTORY_READER_HPP
#define NBODY_COMMON_CORE_TRAJECTORY_READER_HPP

#include "common/core/Bodies.hpp"

#include <fstream>
#include <string>

class TrajectoryReader {
public:
    explicit TrajectoryReader(const std::string& path);

    [[nodiscard]] bool read_next_frame(BodiesAoS& bodies, std::size_t& step, double& time);

private:
    std::ifstream input_;
    std::string pending_line_;
    bool has_pending_line_{false};
};


#endif
