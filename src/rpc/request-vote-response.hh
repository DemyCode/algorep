#pragma once

#include "json/json.hpp"

class RequestVoteResponse
{
public:
    using json = nlohmann::json;

    RequestVoteResponse(int term, bool vote_granted_);
    RequestVoteResponse(const json& serialized_json);
    RequestVoteResponse(const std::string& serialized);

    std::string serialized() const;

private:
    // current_term, for candidate to update itself
    const int term_;
    // true means candidate received vote
    const bool vote_granted_;
};