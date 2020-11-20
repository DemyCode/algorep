//
// Created by mehdi on 20/11/2020.
//

#include "clock.hh"
Clock::Clock()
{
    this->begin = high_resolution_clock::now();
}
float Clock::check()
{
    return duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - begin).count();
}
void Clock::reset()
{
    this->begin = high_resolution_clock::now();
}
