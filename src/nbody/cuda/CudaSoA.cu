#include "nbody/cuda/CudaSoA.hpp"

#include <cuda_runtime.h>

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef CUDA_BLOCK_SIZE
#define CUDA_BLOCK_SIZE 256
#endif

void checkCuda(cudaError_t error, const char* operation) {
    if (error != cudaSuccess) {
        throw std::runtime_error(
            std::string(operation) + ": " + cudaGetErrorString(error));
    }
}

__global__ void computeAccelerationsTiled(
    const double* qx,
    const double* qy,
    const double* qz,
    const double* mass,
    double* ax,
    double* ay,
    double* az,
    std::size_t count,
    double softeningSquared,
    double gravitationalConstant) {
    __shared__ double tileQx[CUDA_BLOCK_SIZE];
    __shared__ double tileQy[CUDA_BLOCK_SIZE];
    __shared__ double tileQz[CUDA_BLOCK_SIZE];
    __shared__ double tileMass[CUDA_BLOCK_SIZE];

    const unsigned int localIndex = threadIdx.x;
    const std::size_t iBody =
        static_cast<std::size_t>(blockIdx.x) * blockDim.x + localIndex;

    double qix = 0.0;
    double qiy = 0.0;
    double qiz = 0.0;
    if (iBody < count) {
        qix = qx[iBody];
        qiy = qy[iBody];
        qiz = qz[iBody];
    }

    double aix = 0.0;
    double aiy = 0.0;
    double aiz = 0.0;

    for (std::size_t tileStart = 0; tileStart < count; tileStart += blockDim.x) {
        const std::size_t jBody = tileStart + localIndex;

        // Consecutive threads cooperatively load one source-body tile.
        if (jBody < count) {
            tileQx[localIndex] = qx[jBody];
            tileQy[localIndex] = qy[jBody];
            tileQz[localIndex] = qz[jBody];
            tileMass[localIndex] = mass[jBody];
        } else {
            tileQx[localIndex] = 0.0;
            tileQy[localIndex] = 0.0;
            tileQz[localIndex] = 0.0;
            tileMass[localIndex] = 0.0;
        }

        __syncthreads();

        const std::size_t remaining = count - tileStart;
        const unsigned int tileSize = remaining < blockDim.x
            ? static_cast<unsigned int>(remaining)
            : blockDim.x;

        if (iBody < count) {
            for (unsigned int k = 0; k < tileSize; ++k) {
                const double dx = tileQx[k] - qix;
                const double dy = tileQy[k] - qiy;
                const double dz = tileQz[k] - qiz;

                const double distanceSquared =
                    dx * dx + dy * dy + dz * dz + softeningSquared;
                const double inverseDistance = 1.0 / sqrt(distanceSquared);
                const double inverseDistanceCubed =
                    inverseDistance * inverseDistance * inverseDistance;
                const double factor = gravitationalConstant
                    * tileMass[k] * inverseDistanceCubed;

                aix += factor * dx;
                aiy += factor * dy;
                aiz += factor * dz;
            }
        }

        // The tile cannot be overwritten until every thread has consumed it.
        __syncthreads();
    }

    if (iBody < count) {
        ax[iBody] = aix;
        ay[iBody] = aiy;
        az[iBody] = aiz;
    }
}

CudaSoA::CudaSoA(const SimulationConfig& config)
    : SimulationNBodyInterface(config),
      accelerations{
          std::vector<double>(bodies_.getN()),
          std::vector<double>(bodies_.getN()),
          std::vector<double>(bodies_.getN())} {
    const std::size_t bytes = bodies_.getN() * sizeof(double);

    try {
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_qx), bytes), "cudaMalloc d_qx");
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_qy), bytes), "cudaMalloc d_qy");
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_qz), bytes), "cudaMalloc d_qz");
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_m), bytes), "cudaMalloc d_m");
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_ax), bytes), "cudaMalloc d_ax");
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_ay), bytes), "cudaMalloc d_ay");
        checkCuda(cudaMalloc(reinterpret_cast<void**>(&d_az), bytes), "cudaMalloc d_az");
    } catch (...) {
        releaseCudaResources();
        throw;
    }
}

CudaSoA::~CudaSoA() {
    releaseCudaResources();
}

void CudaSoA::releaseCudaResources() noexcept {
    if (d_qx != nullptr) cudaFree(d_qx);
    if (d_qy != nullptr) cudaFree(d_qy);
    if (d_qz != nullptr) cudaFree(d_qz);
    if (d_m != nullptr) cudaFree(d_m);
    if (d_ax != nullptr) cudaFree(d_ax);
    if (d_ay != nullptr) cudaFree(d_ay);
    if (d_az != nullptr) cudaFree(d_az);
}

void CudaSoA::initIteration() {
    // The kernel overwrites every acceleration component, so no reset is required.
}

void CudaSoA::computeBodiesAcceleration() {
    const BodiesSoA& data = bodies_.getDataSoA();
    const std::size_t count = bodies_.getN();
    const std::size_t bytes = count * sizeof(double);

    checkCuda(cudaMemcpy(
                  d_qx, data.qx.data(), bytes, cudaMemcpyHostToDevice),
              "cudaMemcpy qx");
    checkCuda(cudaMemcpy(
                  d_qy, data.qy.data(), bytes, cudaMemcpyHostToDevice),
              "cudaMemcpy qy");
    checkCuda(cudaMemcpy(
                  d_qz, data.qz.data(), bytes, cudaMemcpyHostToDevice),
              "cudaMemcpy qz");
    checkCuda(cudaMemcpy(
                  d_m, data.m.data(), bytes, cudaMemcpyHostToDevice),
              "cudaMemcpy mass");

    const unsigned int blocks = static_cast<unsigned int>(
        (count + CUDA_BLOCK_SIZE - 1) / CUDA_BLOCK_SIZE);

    computeAccelerationsTiled<<<blocks, CUDA_BLOCK_SIZE>>>(
        d_qx,
        d_qy,
        d_qz,
        d_m,
        d_ax,
        d_ay,
        d_az,
        count,
        physics_.softening * physics_.softening,
        physics_.gravitational_constant);
    checkCuda(cudaGetLastError(), "computeAccelerationsTiled launch");
    checkCuda(cudaDeviceSynchronize(), "computeAccelerationsTiled execution");

    checkCuda(cudaMemcpy(
                  accelerations.ax.data(), d_ax, bytes, cudaMemcpyDeviceToHost),
              "cudaMemcpy ax");
    checkCuda(cudaMemcpy(
                  accelerations.ay.data(), d_ay, bytes, cudaMemcpyDeviceToHost),
              "cudaMemcpy ay");
    checkCuda(cudaMemcpy(
                  accelerations.az.data(), d_az, bytes, cudaMemcpyDeviceToHost),
              "cudaMemcpy az");
}

void CudaSoA::computeOneIteration() {
    initIteration();
    computeBodiesAcceleration();
    bodies_.updatePositionsAndVelocities(accelerations, config_.dt);
    time_ += config_.dt;
}

const BodiesSoA& CudaSoA::bodies() const {
    return bodies_.getDataSoA();
}