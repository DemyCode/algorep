#pragma once

#include <json.hpp>
#include <string>

class Entry
{
public:
    using json = nlohmann::json;

    Entry(int term, std::string command);
    Entry(const json& serialized_json);
    Entry(const std::string& serialized);

    std::string serialize() const;

private:
    const int term_;
    const std::string command_;
};