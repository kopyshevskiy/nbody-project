#ifndef NBODY_COMMON_UTILS_TIMER_HPP
#define NBODY_COMMON_UTILS_TIMER_HPP

#include <chrono>

class Timer {
public:
    void start() noexcept;
    [[nodiscard]] double elapsed_seconds() const noexcept;

private:
    std::chrono::steady_clock::time_point start_{};
};


#endif
