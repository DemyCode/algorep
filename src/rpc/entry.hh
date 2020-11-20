#pragma once

#include <string>

#include "json/json.hpp"

class Entry
{
public:
    using json = nlohmann::json;

    Entry(int term, std::string command);
    Entry(const json& serialized_json);
    Entry(const std::string& serialized);

    std::string serialize() const;

    int term_;
    std::string command_;
};