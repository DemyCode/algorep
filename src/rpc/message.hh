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

    enum SPEED_TYPE
    {
        LOW = 500,
        MEDIUM = 250,
        HIGH = 0
    };

    Message(int uid, MESSAGE_TYPE message_type, std::string message_content);
    Message(const json& serialized_json);
    Message(const std::string& serialized);

    json serialize_message() const override;

    static SPEED_TYPE parse_speed(const std::string& speed);

    const int uid_;
    const MESSAGE_TYPE message_type_;
    const std::string message_content_;
};