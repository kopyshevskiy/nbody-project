#include "common/core/InitialCondition.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <random>
#include <utility>
#include <vector>

bool is_valid_body(const Body& body) {
    return body.m > 0.0 && std::isfinite(body.m)
        && body.r >= 0.0 && std::isfinite(body.r)
        && std::isfinite(body.qx) && std::isfinite(body.qy)
        && std::isfinite(body.qz) && std::isfinite(body.vx)
        && std::isfinite(body.vy) && std::isfinite(body.vz);
}

BodiesAoS load_initial_conditions(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("cannot open input file: " + path);
    }

    std::vector<Body> bodies;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        std::istringstream row(line);
        row >> std::ws;
        if (row.eof() || row.peek() == '#') {
            continue;
        }

        Body body;
        body.r = 1.0;
        if (!(row >> body.m >> body.qx >> body.qy >> body.qz
                  >> body.vx >> body.vy >> body.vz)
            || !is_valid_body(body)) {
            throw std::runtime_error(
                "invalid body at line " + std::to_string(line_number) + " in " + path);
        }
        row >> std::ws;
        if (!row.eof() && row.peek() != '#') {
            throw std::runtime_error(
                "unexpected data at line " + std::to_string(line_number) + " in " + path);
        }
        bodies.push_back(body);
    }

    if (bodies.empty()) {
        throw std::runtime_error("input file contains no bodies: " + path);
    }

    return BodiesAoS{std::move(bodies)};
}

BodiesAoS generate_circular_orbit(std::size_t count) {
    constexpr double pi = 3.14159265358979323846;
    if (count == 0) {
        throw std::invalid_argument("the number of bodies must be greater than zero");
    }

    std::vector<Body> bodies;
    bodies.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const double angle = 2.0 * pi * static_cast<double>(i) / static_cast<double>(count);
        Body body;
        body.qx = std::cos(angle);
        body.qy = std::sin(angle);
        body.qz = 0.0;
        body.vx = -std::sin(angle);
        body.vy = std::cos(angle);
        body.vz = 0.0;
        body.m = 1.0;
        body.r = 1.0;
        bodies.push_back(body);
    }

    return BodiesAoS{std::move(bodies)};
}

BodiesAoS generate_galaxy(std::size_t count, double gravitational_constant) {
    if (count == 0) {
        throw std::invalid_argument("the number of bodies must be greater than zero");
    }

    constexpr double pi = 3.14159265358979323846;
    constexpr double central_mass = 100.0;
    std::mt19937 generator(0);
    std::uniform_real_distribution<double> radius_distribution(2.0, 10.0);
    std::uniform_real_distribution<double> angle_distribution(0.0, 2.0 * pi);
    std::uniform_real_distribution<double> height_distribution(-0.4, 0.4);
    std::uniform_real_distribution<double> mass_distribution(0.005, 0.08);
    std::uniform_real_distribution<double> speed_distribution(0.85, 1.15);

    std::vector<Body> bodies;
    bodies.reserve(count);

    Body central_body{};
    central_body.m = central_mass;
    central_body.r = 0.4;
    bodies.push_back(central_body);

    for (std::size_t i = 1; i < count; ++i) {
        const double radius = radius_distribution(generator);
        const double angle = angle_distribution(generator);
        const double mass = mass_distribution(generator);
        const double speed = speed_distribution(generator)
            * std::sqrt(gravitational_constant * central_mass / radius);

        Body body{};
        body.m = mass;
        body.r = 0.08 + 3.0 * mass;
        body.qx = radius * std::cos(angle);
        body.qy = radius * std::sin(angle);
        body.qz = height_distribution(generator);
        body.vx = -speed * std::sin(angle);
        body.vy = speed * std::cos(angle);
        body.vz = 0.0;
        bodies.push_back(body);
    }

    return BodiesAoS{std::move(bodies)};
}

BodiesAoS generate_spiral(std::size_t count, double gravitational_constant) {
    if (count == 0) {
        throw std::invalid_argument("the number of bodies must be greater than zero");
    }

    constexpr double pi = 3.14159265358979323846;
    constexpr std::size_t number_of_arms = 4;
    constexpr double number_of_rounds = 0.7;
    constexpr double outer_radius = 8.0;
    std::mt19937 generator(0);
    std::normal_distribution<double> radial_dispersion(0.0, 0.1);
    std::normal_distribution<double> height_distribution(0.0, 0.04);
    std::uniform_real_distribution<double> mass_distribution(0.005, 0.08);
    std::uniform_real_distribution<double> speed_distribution(0.95, 1.05);

    std::vector<Body> bodies;
    bodies.reserve(count);
    double enclosed_mass = 0.0;

    for (std::size_t i = 0; i < count; ++i) {
        const double fraction = static_cast<double>(i) / static_cast<double>(count);
        const double arm_angle = 2.0 * pi * static_cast<double>(i % number_of_arms)
            / static_cast<double>(number_of_arms);
        const double angle = 2.0 * pi * fraction * number_of_rounds + arm_angle;
        const double radius = outer_radius * (fraction + 0.04);
        const double offset = radial_dispersion(generator);
        const double mass = mass_distribution(generator);
        enclosed_mass += mass;

        const double speed = -speed_distribution(generator)
            * std::sqrt(gravitational_constant * enclosed_mass / radius);

        Body body{};
        body.m = mass;
        body.r = 0.06 + 2.0 * mass;
        body.qx = radius * std::cos(angle) + offset * std::cos(angle + pi / 2.0);
        body.qy = radius * std::sin(angle) + offset * std::sin(angle + pi / 2.0);
        body.qz = height_distribution(generator);
        body.vx = -speed * std::sin(angle);
        body.vy = speed * std::cos(angle);
        body.vz = 0.0;
        bodies.push_back(body);
    }

    return BodiesAoS{std::move(bodies)};
}
