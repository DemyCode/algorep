#pragma once

#include <string>

#include "json/json.hpp"
#include "rpc.hh"

class RequestVote : public RPC
{
public:
    using json = nlohmann::json;

    RequestVote(int term, int candidate_id, int last_log_index, int last_log_term);
    RequestVote(const json& serialized);
    RequestVote(const std::string& serialized);

    json serialize_message() const override;

private:
    // candidate's term
    const int term_;
    // candidate requesting vote
    const int candidate_id_;
    // index of candidate's last log entry
    const int last_log_index_;
    // term of candidate's last log entry
    const int last_log_term_;
};