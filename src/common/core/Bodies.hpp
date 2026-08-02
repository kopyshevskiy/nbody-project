#ifndef NBODY_COMMON_CORE_BODIES_HPP
#define NBODY_COMMON_CORE_BODIES_HPP

#include <cstddef>
#include <vector>

template <typename T>
struct dataSoA_t {
    std::vector<T> qx;
    std::vector<T> qy;
    std::vector<T> qz;
    std::vector<T> vx;
    std::vector<T> vy;
    std::vector<T> vz;
    std::vector<T> m;
    std::vector<T> r;
};

template <typename T>
struct dataAoS_t {
    T qx;
    T qy;
    T qz;
    T vx;
    T vy;
    T vz;
    T m;
    T r;
};

template <typename T>
struct accSoA_t {
    std::vector<T> ax;
    std::vector<T> ay;
    std::vector<T> az;
};

template <typename T>
struct accAoS_t {
    T ax;
    T ay;
    T az;
};

using Body = dataAoS_t<double>;
using BodiesAoS = std::vector<dataAoS_t<double>>;
using BodiesSoA = dataSoA_t<double>;
using AccelerationAoS = accAoS_t<double>;
using AccelerationsSoA = accSoA_t<double>;

template <typename T>
class Bodies {
public:
    Bodies() = default;
    explicit Bodies(std::vector<dataAoS_t<T>> bodies);

    std::size_t getN() const;
    unsigned short getPadding() const;

    std::vector<dataAoS_t<T>>& getDataAoS();
    const std::vector<dataAoS_t<T>>& getDataAoS() const;

    dataSoA_t<T>& getDataSoA();
    const dataSoA_t<T>& getDataSoA() const;

    void updatePositionsAndVelocities(
        const std::vector<accAoS_t<T>>& accelerations, T dt);
    void updatePositionsAndVelocities(
        const accSoA_t<T>& accelerations, T dt);

    void syncAoSFromSoA();
    void syncSoAFromAoS();

private:
    std::size_t n_{0};
    std::vector<dataAoS_t<T>> dataAoS_;
    dataSoA_t<T> dataSoA_;
    unsigned short padding_{0};
};

BodiesSoA to_soa(const BodiesAoS& bodies);
BodiesAoS to_aos(const BodiesSoA& bodies);

#endif
