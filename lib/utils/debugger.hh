//
// Created by mehdi on 24/11/2020.
//

#ifndef ALGOREP_DEBUGGER_HH
#define ALGOREP_DEBUGGER_HH

#include <string>
#include <fstream>
#include <iostream>

#include "mpi_classes/process-information.hh"

void debug_write(const std::string& debug_message, float timestamp);

#endif //ALGOREP_DEBUGGER_HH
