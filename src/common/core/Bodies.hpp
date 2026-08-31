#ifndef NBODY_COMMON_CORE_BODIES_HPP
#define NBODY_COMMON_CORE_BODIES_HPP

#include <cstddef>
#include <vector>

/*!
 * \brief Structure of arrays of body characteristics.
 */
template <typename T>
struct dataSoA_t {
    std::vector<T> qx; /*!< Array of positions x. */
    std::vector<T> qy; /*!< Array of positions y. */
    std::vector<T> qz; /*!< Array of positions z. */
    std::vector<T> vx; /*!< Array of velocities x. */
    std::vector<T> vy; /*!< Array of velocities y. */
    std::vector<T> vz; /*!< Array of velocities z. */
    std::vector<T> m;  /*!< Array of masses. */
    std::vector<T> r;  /*!< Array of radiuses. */
};

/*!
 * \brief Structure of body characteristics.
 */
template <typename T>
struct dataAoS_t {
    T qx; /*!< Position x. */
    T qy; /*!< Position y. */
    T qz; /*!< Position z. */
    T vx; /*!< Velocity x. */
    T vy; /*!< Velocity y. */
    T vz; /*!< Velocity z. */
    T m;  /*!< Mass. */
    T r;  /*!< Radius. */
};

/*!
 * \brief Structure of arrays of body accelerations.
 */
template <typename T>
struct accSoA_t {
    std::vector<T> ax; /*!< Array of accelerations x. */
    std::vector<T> ay; /*!< Array of accelerations y. */
    std::vector<T> az; /*!< Array of accelerations z. */
};

/*!
 * \brief Structure of body acceleration.
 */
template <typename T>
struct accAoS_t {
    T ax; /*!< Acceleration x. */
    T ay; /*!< Acceleration y. */
    T az; /*!< Acceleration z. */
};

using Body = dataAoS_t<double>;
using BodiesAoS = std::vector<dataAoS_t<double>>;
using BodiesSoA = dataSoA_t<double>;
using AccelerationAoS = accAoS_t<double>;
using AccelerationsSoA = accSoA_t<double>;

template <typename T>
class Bodies {
public:
    /*!
     * \brief Constructor.
     */
    Bodies() = default;

    /*!
     * \brief Constructor from an AoS body vector.
     */
    explicit Bodies(std::vector<dataAoS_t<T>> bodies);

    /*!
     * \brief Number of physical bodies getter.
     */
    std::size_t getN() const;

    /*!
     * \brief Padding getter.
     */
    unsigned short getPadding() const;

    /*!
     * \brief AoS data getter.
     */
    std::vector<dataAoS_t<T>>& getDataAoS();

    /*!
     * \brief Const AoS data getter.
     */
    const std::vector<dataAoS_t<T>>& getDataAoS() const;

    /*!
     * \brief SoA data getter.
     */
    dataSoA_t<T>& getDataSoA();

    /*!
     * \brief Const SoA data getter.
     */
    const dataSoA_t<T>& getDataSoA() const;

    /*!
     * \brief Update positions and velocities from AoS accelerations.
     */
    void updatePositionsAndVelocities(
        const std::vector<accAoS_t<T>>& accelerations, T dt);

    /*!
     * \brief Update positions and velocities from SoA accelerations.
     */
    void updatePositionsAndVelocities(
        const accSoA_t<T>& accelerations, T dt);

    /*!
     * \brief Synchronize the AoS storage from the SoA storage.
     */
    void syncAoSFromSoA();

    /*!
     * \brief Synchronize the SoA storage from the AoS storage.
     */
    void syncSoAFromAoS();

private:
    std::size_t n_{0};                  /*!< Number of bodies. */
    std::vector<dataAoS_t<T>> dataAoS_; /*!< Array of body structures. */
    dataSoA_t<T> dataSoA_;              /*!< Structure of body arrays. */
    unsigned short padding_{0};         /*!< Number of padding bodies. */
};

/*!
 * \brief Convert an AoS body vector to SoA storage.
 */
BodiesSoA to_soa(const BodiesAoS& bodies);

/*!
 * \brief Convert SoA storage to an AoS body vector.
 */
BodiesAoS to_aos(const BodiesSoA& bodies);

#endif
