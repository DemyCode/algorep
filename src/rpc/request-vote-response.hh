#pragma once

#include "json/json.hpp"
#include "rpc.hh"

class RequestVoteResponse : public RPC
{
public:
    using json = nlohmann::json;

    RequestVoteResponse(int term, bool vote_granted_);
    RequestVoteResponse(int term, const json& serialized_json);
    RequestVoteResponse(int term, const std::string& serialized);

    json serialize_message() const override;

    // true means candidate received vote
    const bool vote_granted_;
};