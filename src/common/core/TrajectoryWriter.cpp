#include "common/core/TrajectoryWriter.hpp"

#include <iomanip>
#include <limits>
#include <stdexcept>

TrajectoryWriter::TrajectoryWriter(const std::string& path) : output_(path) {
    if (!output_) {
        throw std::runtime_error("cannot open trajectory output: " + path);
    }
    output_ << std::setprecision(std::numeric_limits<double>::max_digits10);
    output_ << "step,time,id,mass,x,y,z,vx,vy,vz,radius\n";
}

void TrajectoryWriter::write_step(std::size_t step, double time, const BodiesAoS& bodies) {
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        const Body& body = bodies[i];
        output_ << step << ',' << time << ',' << i << ',' << body.m << ','
                << body.qx << ',' << body.qy << ',' << body.qz << ','
                << body.vx << ',' << body.vy << ',' << body.vz << ','
                << body.r << '\n';
    }
}

void TrajectoryWriter::write_step(std::size_t step, double time, const BodiesSoA& bodies) {
    for (std::size_t i = 0; i < bodies.m.size(); ++i) {
        output_ << step << ',' << time << ',' << i << ',' << bodies.m[i] << ','
                << bodies.qx[i] << ',' << bodies.qy[i] << ',' << bodies.qz[i] << ','
                << bodies.vx[i] << ',' << bodies.vy[i] << ',' << bodies.vz[i] << ','
                << bodies.r[i] << '\n';
    }
}
