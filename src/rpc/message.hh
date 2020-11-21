#pragma once

#include "rpc.hh"

class Message : public RPC
{
public:
    using json = nlohmann::json;

    enum MESSAGE_TYPE
    {
        CLIENT_CREATE_NEW_ENTRY,
        SERVER_SET_SPEED,
        SERVER_CRASH,
        SERVER_START,
        SERVER_RECOVER
    };

    Message(MESSAGE_TYPE message_type, std::string message_content);
    Message(const json& serialized_json);
    Message(const std::string& serialized);

    json serialize_message() const override;

    const MESSAGE_TYPE message_type_;
    const std::string message_content_;
};