#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "mpi_classes/process-information.hh"

/**
 * @brief Function to write in a debug file, only works in Debug mode
 * @param debug_message
 */
void debug_write(const std::string& debug_message);
