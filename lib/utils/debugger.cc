#include "debugger.hh"

void debug_write(const std::string& debug_message, float timestamp)
{
    std::ofstream log_file("debug/debug_" + std::to_string(ProcessInformation::instance().rank_) + ".txt",
                           std::ofstream::out | std::ofstream::app);
    if (log_file.is_open())
    {
        log_file << timestamp << " : " << debug_message << std::endl;
        log_file.close();
    }
    else
        std::cerr << "Unable to open file : "
                  << "debug_" + std::to_string(ProcessInformation::instance().rank_) + ".txt" << std::endl;
}