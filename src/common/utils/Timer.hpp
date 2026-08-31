#ifndef NBODY_COMMON_UTILS_TIMER_HPP
#define NBODY_COMMON_UTILS_TIMER_HPP

#include <chrono>

class Timer {
public:
    /*!
     * \brief Start the timer.
     */
    void start() noexcept;

    /*!
     * \brief Elapsed time getter in seconds.
     */
    double elapsed_seconds() const noexcept;

private:
    std::chrono::steady_clock::time_point start_{}; /*!< Start time point. */
};


#endif
