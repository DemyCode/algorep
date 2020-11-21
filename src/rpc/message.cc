#include <utility>

#include "message.hh"

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

Message::json Message::serialize_message() const
{
    json json_object;

    json_object["message_type"] = this->message_type_;
    json_object["message_content"] = this->message_content_;

    return json_object;
}
