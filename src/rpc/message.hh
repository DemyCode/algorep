#pragma once

#include "rpc.hh"

class Message : public RPC
{
public:
    /// type of the message
    enum MESSAGE_TYPE
    {
        CLIENT_CREATE_NEW_ENTRY,
        CLIENT_START,
        CLIENT_WAIT_FINISH,
        SERVER_TIMEOUT,
        SERVER_PRINT_LOCAL_LOG,
        PROCESS_SET_SPEED,
        PROCESS_CRASH,
        PROCESS_RECOVER,
        PROCESS_STOP,
    };

    /// speed type for the message t be handled
    enum SPEED_TYPE
    {
        LOW = 500,
        MEDIUM = 250,
        HIGH = 0
    };

    /** @brief Constructor
     ** @param message_type type of the message
     ** @param message_content content of the message as string
    */
    Message(MESSAGE_TYPE message_type, std::string message_content);
    /** @brief Constructor
     ** @param serialized_json content of the message as json
    */
    Message(const json& serialized_json);
    /** @brief Constructor
     ** @param serialized content of the message serialized as string
    */
    Message(const std::string& serialized);

    /** @brief serializes the message in json
    */
    json serialize_message() const override;

    /** @brief Constructor
     ** @param speed speed for the parsing
    */
    static SPEED_TYPE parse_speed(const std::string& speed);

    /// type of the message
    const MESSAGE_TYPE message_type_;
    /// content of the message
    const std::string message_content_;
};

class MessageResponse : public RPC
{
public:
    MessageResponse(bool success);
    MessageResponse(const json& serialized_json);
    MessageResponse(const std::string& serialized);

    json serialize_message() const override;

    const bool success_;
};