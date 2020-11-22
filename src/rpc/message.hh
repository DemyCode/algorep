#pragma once

#include "rpc.hh"

class Message : public RPC
{
public:
    using json = nlohmann::json;

    enum MESSAGE_TYPE
    {
        CLIENT_CREATE_NEW_ENTRY,
        CLIENT_START,
        PROCESS_SET_SPEED,
        PROCESS_CRASH,
        PROCESS_RECOVER
    };

    Message(int uid, MESSAGE_TYPE message_type, std::string message_content);
    Message(const json& serialized_json);
    Message(const std::string& serialized);

    json serialize_message() const override;

    const int uid_;
    const MESSAGE_TYPE message_type_;
    const std::string message_content_;
};