#pragma once

#include <string>

#include "json/json.hpp"
#include "rpc.hh"

class RequestVote : public RPC
{
public:
    RequestVote(int term, size_t candidate_id, size_t last_log_index, int last_log_term);
    RequestVote(int term, const json& serialized);
    RequestVote(int term, const std::string& serialized);

    json serialize_message() const override;

    // candidate requesting vote
    const size_t candidate_id_;
    // index of candidate's last log entry
    const int last_log_index_;
    // term of candidate's last log entry
    const size_t last_log_term_;
};

class RequestVoteResponse : public RPC
{
public:
    RequestVoteResponse(int term, bool vote_granted_);
    RequestVoteResponse(int term, const json& serialized_json);
    RequestVoteResponse(int term, const std::string& serialized);

    json serialize_message() const override;

    // true means candidate received vote
    const bool vote_granted_;
};