#include "common/core/Bodies.hpp"

#include <utility>

template <typename T>
Bodies<T>::Bodies(std::vector<dataAoS_t<T>> bodies)
    : n_(bodies.size()),
      dataAoS_(std::move(bodies)),
      dataSoA_(to_soa(dataAoS_)) {}

template <typename T>
std::size_t Bodies<T>::getN() const {
    return n_;
}

template <typename T>
unsigned short Bodies<T>::getPadding() const {
    return padding_;
}

template <typename T>
std::vector<dataAoS_t<T>>& Bodies<T>::getDataAoS() {
    return dataAoS_;
}

template <typename T>
const std::vector<dataAoS_t<T>>& Bodies<T>::getDataAoS() const {
    return dataAoS_;
}

template <typename T>
dataSoA_t<T>& Bodies<T>::getDataSoA() {
    return dataSoA_;
}

template <typename T>
const dataSoA_t<T>& Bodies<T>::getDataSoA() const {
    return dataSoA_;
}

template <typename T>
void Bodies<T>::updatePositionsAndVelocities(
    const std::vector<accAoS_t<T>>& accelerations, T dt) {
    for (std::size_t i = 0; i < dataAoS_.size(); ++i) {
        dataAoS_[i].qx += dataAoS_[i].vx * dt;
        dataAoS_[i].qy += dataAoS_[i].vy * dt;
        dataAoS_[i].qz += dataAoS_[i].vz * dt;
        dataAoS_[i].vx += accelerations[i].ax * dt;
        dataAoS_[i].vy += accelerations[i].ay * dt;
        dataAoS_[i].vz += accelerations[i].az * dt;
    }
    syncSoAFromAoS();
}

template <typename T>
void Bodies<T>::updatePositionsAndVelocities(
    const accSoA_t<T>& accelerations, T dt) {
    for (std::size_t i = 0; i < dataSoA_.m.size(); ++i) {
        dataSoA_.qx[i] += dataSoA_.vx[i] * dt;
        dataSoA_.qy[i] += dataSoA_.vy[i] * dt;
        dataSoA_.qz[i] += dataSoA_.vz[i] * dt;
        dataSoA_.vx[i] += accelerations.ax[i] * dt;
        dataSoA_.vy[i] += accelerations.ay[i] * dt;
        dataSoA_.vz[i] += accelerations.az[i] * dt;
    }
    syncAoSFromSoA();
}

template <typename T>
void Bodies<T>::syncAoSFromSoA() {
    dataAoS_ = to_aos(dataSoA_);
}

template <typename T>
void Bodies<T>::syncSoAFromAoS() {
    dataSoA_ = to_soa(dataAoS_);
}

BodiesSoA to_soa(const BodiesAoS& bodies) {
    BodiesSoA result;
    result.qx.resize(bodies.size());
    result.qy.resize(bodies.size());
    result.qz.resize(bodies.size());
    result.vx.resize(bodies.size());
    result.vy.resize(bodies.size());
    result.vz.resize(bodies.size());
    result.m.resize(bodies.size());
    result.r.resize(bodies.size());
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        result.qx[i] = bodies[i].qx;
        result.qy[i] = bodies[i].qy;
        result.qz[i] = bodies[i].qz;
        result.vx[i] = bodies[i].vx;
        result.vy[i] = bodies[i].vy;
        result.vz[i] = bodies[i].vz;
        result.m[i] = bodies[i].m;
        result.r[i] = bodies[i].r;
    }
    return result;
}

BodiesAoS to_aos(const BodiesSoA& bodies) {
    BodiesAoS result(bodies.m.size());
    for (std::size_t i = 0; i < bodies.m.size(); ++i) {
        result[i].qx = bodies.qx[i];
        result[i].qy = bodies.qy[i];
        result[i].qz = bodies.qz[i];
        result[i].vx = bodies.vx[i];
        result[i].vy = bodies.vy[i];
        result[i].vz = bodies.vz[i];
        result[i].m = bodies.m[i];
        result[i].r = bodies.r[i];
    }
    return result;
}

template class Bodies<double>;
