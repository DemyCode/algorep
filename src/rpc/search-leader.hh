#pragma once

#include "rpc.hh"

class SearchLeader : public RPC
{
public:
    ///Constructor
    SearchLeader(size_t old_leader);
    ///Constructor
    SearchLeader(const json& serialized_json);
    ///Constructor
    SearchLeader(const std::string& serialized);

    /**
     * @brief Serialize the message into a json
     * 
     * @return json 
     */
    json serialize_message() const override;

    ///Id of the old leader
    const size_t old_leader_;
};

class SearchLeaderResponse : public RPC
{
public:
    ///Constructor
    SearchLeaderResponse(size_t new_leader);
    ///Constructor
    SearchLeaderResponse(const json& serialized_json);
    ///Constructor
    SearchLeaderResponse(const std::string& serialized);

    /**
     * @brief Serialize the message into a json
     * 
     * @return json 
     */
    json serialize_message() const override;

    ///Id of the new leader
    const size_t new_leader_;
};