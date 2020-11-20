#include "clock.hh"

Clock::Clock()
    : begin_(high_resolution_clock_t::now())
{}

float Clock::check() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock_t::now() - this->begin_).count();
}

void Clock::reset()
{
    this->begin_ = high_resolution_clock_t::now();
}
