#pragma once

#include <json.hpp>
#include <string>

using json = nlohmann::json;

class RequestVote
{
public:
    RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm);
    RequestVote(std::string serialized);
    std::string serialize();

    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;
};