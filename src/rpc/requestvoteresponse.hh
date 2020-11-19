#pragma once

#include <json.hpp>

using json = nlohmann::json;

class RequestVoteResponse
{
    RequestVoteResponse(int term, bool success);
    RequestVoteResponse(std::string serialized);
    std::string serialized();

    int term;
    bool voteGranted;
};