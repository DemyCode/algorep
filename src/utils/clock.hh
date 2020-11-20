//
// Created by mehdi on 20/11/2020.
//

#ifndef ALGOREP_CLOCK_HH
#define ALGOREP_CLOCK_HH

#include <chrono>

using namespace std::chrono;

class Clock
{
public:
    Clock();
    float check();
    void reset();
    time_point<high_resolution_clock> begin;
};

#endif // ALGOREP_CLOCK_HH
