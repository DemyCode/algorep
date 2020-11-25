#pragma once

#include "rpc.hh"

class GetState : public RPC
{
public:
    ///Constructor
    GetState(int process_rank);
    ///Constructor
    GetState(const json& serialized_json);
    ///Constructor
    GetState(const std::string& serialized);

    /** @brief Serializes the message in json
    */
    json serialize_message() const override;

    /// rank of this process
    const size_t process_rank_;
};

class GetStateResponse : public RPC
{
public:
    /// state of this process
    enum STATE
    {
        FOLLOWER,
        CANDIDATE,
        LEADER,
        STOPPED
    };

    /// Constructor
    GetStateResponse(STATE state);
    /// Constructor
    GetStateResponse(const json& serialized_json);
    /// Constructor
    GetStateResponse(const std::string& serialized);

    /** @brief serializes the message in json
    */
    json serialize_message() const override;

    /// state of the process
    const STATE state_;
};