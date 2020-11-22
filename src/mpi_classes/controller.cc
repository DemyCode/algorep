#include "controller.hh"

#include <string>
#include <iostream>

void Controller::run()
{
    for (std::string line; std::getline(std::cin, line);)
    {
        std::cout << "log: " << line << std::endl;
    }
}
