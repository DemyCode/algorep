#include "debugger.hh"

#include <chrono>

void debug_write(const std::string& debug_message)
{
    std::ofstream log_file("debug/debug_" + std::to_string(ProcessInformation::instance().rank_) + ".txt",
                           std::ofstream::out | std::ofstream::app);
    if (log_file.is_open())
    {
        log_file << std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count()
                 << ProcessInformation::instance().rank_ << " - "
                 << debug_message << " - " << ProcessInformation::instance().rank_ << std::endl;
        log_file.close();
    }
    else
        std::cerr << "Unable to open file : "
                  << "debug_" + std::to_string(ProcessInformation::instance().rank_) + ".txt" << std::endl;
}