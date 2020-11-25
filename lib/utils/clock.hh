#pragma once

#include <chrono>
#include <thread>

class Clock
{
public:
    using high_resolution_clock_t = std::chrono::high_resolution_clock;
    using time_point_t = std::chrono::time_point<high_resolution_clock_t>;

    /** @brief Constructor
     */
    Clock();

    /** @brief check the current milliseconds of the clock
     ** @return the milliseconds
     */
    float check() const;

    /** @brief reset the clock
     */
    void reset();

    /** @brief sleep the program
     ** @param milliseconds milliseconds to sleep
     */
    static void wait(int milliseconds);

private:
    /// time point of the clock
    time_point_t begin_;
};