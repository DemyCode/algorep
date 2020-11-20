#include "lib-clock.hh"

#include <chrono>
#include <thread>

void wait(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}