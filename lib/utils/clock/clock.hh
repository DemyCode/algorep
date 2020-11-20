#pragma once

#include <chrono>

class Clock
{
public:
    using high_resolution_clock_t = std::chrono::high_resolution_clock;
    using time_point_t = std::chrono::time_point<high_resolution_clock_t>;
    
    Clock();
    float check() const;
    void reset();

private:
    time_point_t begin_;
};