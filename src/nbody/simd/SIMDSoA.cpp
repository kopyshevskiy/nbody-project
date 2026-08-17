#include "nbody/simd/SIMDSoA.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

#if defined(NBODY_ENABLE_AVX2)
#include <immintrin.h>
#endif

void compute_one_acceleration(
    const BodiesSoA& bodies,
    const PhysicsParameters& physics,
    std::size_t body_index,
    double& acceleration_x,
    double& acceleration_y,
    double& acceleration_z) {
    const std::size_t count = bodies.m.size();
    const double* mass = bodies.m.data();
    const double* x = bodies.qx.data();
    const double* y = bodies.qy.data();
    const double* z = bodies.qz.data();
    const double softening_squared = physics.softening * physics.softening;
    const double xi = x[body_index];
    const double yi = y[body_index];
    const double zi = z[body_index];

    acceleration_x = 0.0;
    acceleration_y = 0.0;
    acceleration_z = 0.0;
    for (std::size_t j = 0; j < count; ++j) {
        const double dx = x[j] - xi;
        const double dy = y[j] - yi;
        const double dz = z[j] - zi;
        const double distance_squared =
            dx * dx + dy * dy + dz * dz + softening_squared;
        const double inverse_distance = 1.0 / std::sqrt(distance_squared);
        const double inverse_distance_cubed =
            inverse_distance * inverse_distance * inverse_distance;
        const double factor =
            physics.gravitational_constant * mass[j] * inverse_distance_cubed;

        acceleration_x += dx * factor;
        acceleration_y += dy * factor;
        acceleration_z += dz * factor;
    }
}

void SIMDSoA::initIteration() {
    const std::size_t count = bodies_.getN();
    for (std::size_t iBody = 0; iBody < count; ++iBody) {
        accelerations.ax[iBody] = 0.0;
        accelerations.ay[iBody] = 0.0;
        accelerations.az[iBody] = 0.0;
    }
}

void SIMDSoA::computeBodiesAcceleration() {
    const BodiesSoA& bodies = bodies_.getDataSoA();
#if defined(NBODY_ENABLE_AVX2)
    const std::size_t count = bodies.m.size();
    const double* mass = bodies.m.data();
    const double* x = bodies.qx.data();
    const double* y = bodies.qy.data();
    const double* z = bodies.qz.data();
    double* ax = accelerations.ax.data();
    double* ay = accelerations.ay.data();
    double* az = accelerations.az.data();
    const double softening_squared = physics_.softening * physics_.softening;
    const __m256d gravitational_constant =
        _mm256_set1_pd(physics_.gravitational_constant);
    const __m256d softening = _mm256_set1_pd(softening_squared);
    const __m256d one = _mm256_set1_pd(1.0);

    for (std::size_t i = 0; i < count; i += 4) {
        const __m256d xi = _mm256_loadu_pd(x + i);
        const __m256d yi = _mm256_loadu_pd(y + i);
        const __m256d zi = _mm256_loadu_pd(z + i);
        __m256d acceleration_x = _mm256_setzero_pd();
        __m256d acceleration_y = _mm256_setzero_pd();
        __m256d acceleration_z = _mm256_setzero_pd();

        for (std::size_t j = 0; j < count; ++j) {
            const __m256d dx = _mm256_sub_pd(_mm256_set1_pd(x[j]), xi);
            const __m256d dy = _mm256_sub_pd(_mm256_set1_pd(y[j]), yi);
            const __m256d dz = _mm256_sub_pd(_mm256_set1_pd(z[j]), zi);

            // FMA computes the squared distance with one multiply-add per component.
            __m256d distance_squared = _mm256_fmadd_pd(dx, dx, softening);
            distance_squared = _mm256_fmadd_pd(dy, dy, distance_squared);
            distance_squared = _mm256_fmadd_pd(dz, dz, distance_squared);
            const __m256d inverse_distance =
                _mm256_div_pd(one, _mm256_sqrt_pd(distance_squared));
            const __m256d inverse_distance_cubed = _mm256_mul_pd(
                _mm256_mul_pd(inverse_distance, inverse_distance), inverse_distance);
            const __m256d factor = _mm256_mul_pd(
                gravitational_constant,
                _mm256_mul_pd(_mm256_set1_pd(mass[j]), inverse_distance_cubed));

            // FMA multiplies the interaction factor and accumulates the acceleration.
            acceleration_x = _mm256_fmadd_pd(factor, dx, acceleration_x);
            acceleration_y = _mm256_fmadd_pd(factor, dy, acceleration_y);
            acceleration_z = _mm256_fmadd_pd(factor, dz, acceleration_z);
        }

        _mm256_storeu_pd(ax + i, acceleration_x);
        _mm256_storeu_pd(ay + i, acceleration_y);
        _mm256_storeu_pd(az + i, acceleration_z);
    }
#else
    for (std::size_t iBody = 0; iBody < bodies.m.size(); ++iBody) {
        compute_one_acceleration(
            bodies,
            physics_,
            iBody,
            accelerations.ax[iBody],
            accelerations.ay[iBody],
            accelerations.az[iBody]);
    }
#endif
}

SIMDSoA::SIMDSoA(const SimulationConfig& config)
    : SimulationNBodyInterface(config),
      accelerations{
          std::vector<double>(bodies_.getN()),
          std::vector<double>(bodies_.getN()),
          std::vector<double>(bodies_.getN())} {
    if (bodies_.getN() % 4 != 0) {
        throw std::invalid_argument(
            "SIMDSoA requires a number of bodies multiple of four");
    }
}

void SIMDSoA::computeOneIteration() {
    initIteration();
    computeBodiesAcceleration();
    bodies_.updatePositionsAndVelocities(accelerations, config_.dt);
    time_ += config_.dt;
}

const BodiesSoA& SIMDSoA::bodies() const {
    return bodies_.getDataSoA();
}
