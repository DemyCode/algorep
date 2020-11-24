#include "message.hh"

#include <utility>

Message::Message(MESSAGE_TYPE message_type, std::string message_content)
    : RPC(-1, RPC::RPC_TYPE::MESSAGE)
    , message_type_(message_type)
    , message_content_(std::move(message_content))
{}

Message::Message(const json& serialized_json)
    : Message(serialized_json["message_type"], serialized_json["message_content"])
{}

Message::Message(const std::string& serialized)
    : Message(json::parse(serialized))
{}

RPC::json Message::serialize_message() const
{
    json json_object;

    json_object["message_type"] = this->message_type_;
    json_object["message_content"] = this->message_content_;

    return json_object;
}

Message::SPEED_TYPE Message::parse_speed(const std::string& speed)
{
    if (speed == "low")
        return SPEED_TYPE::LOW;
    if (speed == "medium")
        return SPEED_TYPE::MEDIUM;
    if (speed == "high")
        return SPEED_TYPE::HIGH;

    return SPEED_TYPE::HIGH;
}

MessageResponse::MessageResponse(bool success)
    : RPC(-1, RPC::RPC_TYPE::MESSAGE_RESPONSE)
    , success_(success)
{}

MessageResponse::MessageResponse(const json& serialized_json)
    : MessageResponse(serialized_json["success"].get<bool>())
{}

MessageResponse::MessageResponse(const std::string& serialized)
    : MessageResponse(json::parse(serialized))
{}

RPC::json MessageResponse::serialize_message() const
{
    json json_object;

    json_object["success"] = this->success_;

    return json_object;
}