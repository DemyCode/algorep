#pragma once

#include <json.hpp>
#include <string>

using json = nlohmann::json;

class AppendEntriesResponse
{
    AppendEntriesResponse(int term, bool success);
    AppendEntriesResponse(std::string serialized);
    std::string serialized();

    int term;
    bool success;
};
