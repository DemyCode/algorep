#pragma once

#include <string>

#include "json/json.hpp"
#include "rpc.hh"

class RequestVote : public RPC
{
public:
    /**
     * @brief Message to Request a Vote, sent from Candidate
     * @param term term of the sender
     * @param candidate_id rank of the sender
     * @param last_log_index last log index of the sender
     * @param last_log_term last log term of the sender
     */
    ///Constructor
    RequestVote(int term, size_t candidate_id, size_t last_log_index, int last_log_term);
    ///Constructor
    RequestVote(int term, const json& serialized);
    ///Constructor
    RequestVote(int term, const std::string& serialized);

    /**
     * @brief Serialize the message
     * 
     * @return json 
     */
    json serialize_message() const override;

    /// candidate requesting vote
    const size_t candidate_id_;
    /// index of candidate's last log entry
    const int last_log_index_;
    /// term of candidate's last log entry
    const int last_log_term_;
};

class RequestVoteResponse : public RPC
{
public:
    /**
     * @brief Response to RequestVote, send to Candidate
     * @param term
     * @param vote_granted_
     */
     ////Constructor
    RequestVoteResponse(int term, bool vote_granted_);
    ///Constructor
    RequestVoteResponse(int term, const json& serialized_json);
    ///Constructor
    RequestVoteResponse(int term, const std::string& serialized);

    /**
     * @brief Serialize the message
     * 
     * @return json 
     */
    json serialize_message() const override;

    /// true means candidate received vote
    const bool vote_granted_;
};