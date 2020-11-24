#pragma once

#include <string>

#include "json/json.hpp"
#include "rpc/rpc.hh"

class Entry
{
public:
    using json = nlohmann::json;

    Entry(int term, std::string command);
    Entry(const json& serialized_json);
    Entry(const std::string& serialized);

    json serialize_message() const;

    size_t term_;
    std::string command_;
};