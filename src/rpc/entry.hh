#pragma once

#include <json.hpp>
#include <string>

using json = nlohmann::json;

class Entry
{
public:
    Entry(int term, std::string command);
    Entry(std::string serialized);
    std::string serialize();

    int term;
    std::string command;
};