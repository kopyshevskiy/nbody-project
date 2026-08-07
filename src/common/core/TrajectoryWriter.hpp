#ifndef NBODY_COMMON_CORE_TRAJECTORY_WRITER_HPP
#define NBODY_COMMON_CORE_TRAJECTORY_WRITER_HPP

#include "common/core/Bodies.hpp"
#include "common/core/Bodies.hpp"

#include <fstream>
#include <string>

class TrajectoryWriter {
public:
    explicit TrajectoryWriter(const std::string& path);

    void write_step(std::size_t step, double time, const BodiesAoS& bodies);
    void write_step(std::size_t step, double time, const BodiesSoA& bodies);

private:
    std::ofstream output_;
};


#endif
