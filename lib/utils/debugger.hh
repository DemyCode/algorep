#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "mpi_classes/process-information.hh"

void debug_write(const std::string& debug_message, float timestamp);
