#include "common/core/Physics.hpp"

#include <cmath>

AccelerationAoS acceleration_from_body(
    const Body& target, const Body& source, const PhysicsParameters& params) {
    const double dx = source.qx - target.qx;
    const double dy = source.qy - target.qy;
    const double dz = source.qz - target.qz;
    const double r2 = dx * dx + dy * dy + dz * dz
        + params.softening * params.softening;
    const double inv_r = 1.0 / std::sqrt(r2);
    const double inv_r3 = inv_r * inv_r * inv_r;
    const double factor = params.gravitational_constant * source.m * inv_r3;
    return AccelerationAoS{dx * factor, dy * factor, dz * factor};
}

std::vector<AccelerationAoS> compute_accelerations_aos(
    const BodiesAoS& bodies, const PhysicsParameters& params) {
    std::vector<AccelerationAoS> accelerations(bodies.size());
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        double ax = 0.0;
        double ay = 0.0;
        double az = 0.0;
        for (std::size_t j = 0; j < bodies.size(); ++j) {
            if (i != j) {
                const AccelerationAoS acceleration =
                    acceleration_from_body(bodies[i], bodies[j], params);
                ax += acceleration.ax;
                ay += acceleration.ay;
                az += acceleration.az;
            }
        }
        accelerations[i] = AccelerationAoS{ax, ay, az};
    }
    return accelerations;
}

AccelerationsSoA compute_accelerations_soa(
    const BodiesSoA& bodies, const PhysicsParameters& params) {
    const std::vector<double>& mass = bodies.m;
    const std::vector<double>& x = bodies.qx;
    const std::vector<double>& y = bodies.qy;
    const std::vector<double>& z = bodies.qz;

    AccelerationsSoA accelerations{
        std::vector<double>(bodies.m.size()),
        std::vector<double>(bodies.m.size()),
        std::vector<double>(bodies.m.size())};

    for (std::size_t i = 0; i < bodies.m.size(); ++i) {
        double ax = 0.0;
        double ay = 0.0;
        double az = 0.0;

        for (std::size_t j = 0; j < bodies.m.size(); ++j) {
            if (i != j) {
                const double dx = x[j] - x[i];
                const double dy = y[j] - y[i];
                const double dz = z[j] - z[i];
                const double r2 = dx * dx + dy * dy + dz * dz
                    + params.softening * params.softening;
                const double inv_r = 1.0 / std::sqrt(r2);
                const double inv_r3 = inv_r * inv_r * inv_r;
                const double factor =
                    params.gravitational_constant * mass[j] * inv_r3;

                ax += dx * factor;
                ay += dy * factor;
                az += dz * factor;
            }
        }

        accelerations.ax[i] = ax;
        accelerations.ay[i] = ay;
        accelerations.az[i] = az;
    }

    return accelerations;
}
