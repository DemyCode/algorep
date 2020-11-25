#pragma once

#include "rpc.hh"

class GetState : public RPC
{
public:
    /** @brief Constructor
     ** @param process_rank rank of the process to send the message to
    */
    GetState(int process_rank);
    /** @brief Constructor
     ** @param serialized_json content of the message as json
    */
    GetState(const json& serialized_json);
    /** @brief Constructor
     ** @param serialized content of the message serialized as string
    */
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

    /** @brief Constructor
     ** @param state state of the process
    */
    GetStateResponse(STATE state);
    /** @brief Constructor
     ** @param serialized_json content of the message as json
    */
    GetStateResponse(const json& serialized_json);
    /** @brief Constructor
     ** @param serialized content of the message serialized as string
    */
    GetStateResponse(const std::string& serialized);

    /** @brief serializes the message in json
    */
    json serialize_message() const override;

    /// state of the process
    const STATE state_;
};