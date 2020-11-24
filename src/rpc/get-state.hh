#pragma once

#include "rpc.hh"

class GetState : public RPC
{
public:
    GetState(int process_rank);
    GetState(const json& serialized_json);
    GetState(const std::string& serialized);

    json serialize_message() const override;

    const size_t process_rank_;
};

class GetStateResponse : public RPC
{
public:
    enum STATE
    {
        FOLLOWER,
        CANDIDATE,
        LEADER,
        STOPPED
    };

    GetStateResponse(STATE state);
    GetStateResponse(const json& serialized_json);
    GetStateResponse(const std::string& serialized);

    json serialize_message() const override;

    const STATE state_;
};