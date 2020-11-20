#pragma once

#include "json/json.hpp"
#include "rpc.hh"

class RequestVoteResponse : public RPC
{
public:
    using json = nlohmann::json;

    RequestVoteResponse(int term, bool vote_granted_);
    RequestVoteResponse(const json& serialized_json);
    RequestVoteResponse(const std::string& serialized);

    json serialize_message() const override;

    // current_term, for candidate to update itself
    const int term_;
    // true means candidate received vote
    const bool vote_granted_;
};