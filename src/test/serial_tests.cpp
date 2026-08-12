#include "common/core/Bodies.hpp"
#include "common/core/InitialCondition.hpp"
#include "common/core/Physics.hpp"
#include "common/core/SimulationConfig.hpp"
#include "nbody/serial/SerialAoS.hpp"

#include <cmath>
#include <stdexcept>

void require(bool condition) {
    if (!condition) {
        throw std::runtime_error("serial test failed");
    }
}

int main() {
    {
        const BodiesAoS bodies = generate_circular_orbit(4);
        require(bodies.size() == 4);

        const BodiesSoA soa = to_soa(bodies);
        const BodiesAoS round_trip = to_aos(soa);
        require(round_trip.size() == bodies.size());
        require(std::abs(round_trip[1].qx - bodies[1].qx) < 1.0e-12);
        require(std::abs(round_trip[1].vy - bodies[1].vy) < 1.0e-12);
    }

    {
        const BodiesAoS bodies = generate_spiral(32, 1.0);
        require(bodies.size() == 32);
        for (const Body& body : bodies) {
            require(body.m > 0.0);
            require(body.r > 0.0);
            require(std::isfinite(body.qx));
            require(std::isfinite(body.qy));
            require(std::isfinite(body.qz));
            require(std::isfinite(body.vx));
            require(std::isfinite(body.vy));
        }
    }

    {
        SimulationConfig config;
        config.dt = 0.0;
        bool rejected = false;
        try {
            validate_config(config);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        require(rejected);
    }

    {
        BodiesAoS bodies{std::vector<Body>{
            Body{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 1.0},
            Body{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 1.0},
        }};
        const auto acceleration = compute_accelerations_aos(
            bodies, PhysicsParameters{1.0, 0.0});
        require(std::abs(acceleration[0].ax - 3.0) < 1.0e-12);
        require(std::abs(acceleration[1].ax + 2.0) < 1.0e-12);
    }

    {
        Bodies<double> bodies(std::vector<Body>{
            Body{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 1.0, 1.0},
        });
        const std::vector<AccelerationAoS> accelerations{
            AccelerationAoS{2.0, 4.0, 6.0},
        };
        bodies.updatePositionsAndVelocities(accelerations, 0.5);

        const Body& body = bodies.getDataAoS()[0];
        require(std::abs(body.qx - 3.0) < 1.0e-12);
        require(std::abs(body.qy - 4.5) < 1.0e-12);
        require(std::abs(body.qz - 6.0) < 1.0e-12);
        require(std::abs(body.vx - 5.0) < 1.0e-12);
        require(std::abs(body.vy - 7.0) < 1.0e-12);
        require(std::abs(body.vz - 9.0) < 1.0e-12);
    }

    {
        SimulationConfig config;
        config.implementation = Implementation::SerialAoS;
        config.num_bodies = 2;
        config.num_steps = 2;
        config.dt = 0.01;
        config.write_trajectory = false;
        SerialAoS solver(config);
        solver.run();
        require(solver.bodies().size() == 2);
    }

    return 0;
}
