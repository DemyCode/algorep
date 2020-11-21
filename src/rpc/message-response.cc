#include "message-response.hh"

MessageResponse::MessageResponse(int uid, bool success)
    : RPC(-1, RPC::RPC_TYPE::MESSAGE_RESPONSE)
    , uid_(uid)
    , success_(success)
{}

MessageResponse::MessageResponse(const json& serialized_json)
    : MessageResponse(serialized_json["uid"], serialized_json["success"])
{}

MessageResponse::MessageResponse(const std::string& serialized)
    : MessageResponse(json::parse(serialized))
{}

MessageResponse::json MessageResponse::serialize_message() const
{
    json json_object;

    json_object["uid"] = this->uid_;
    json_object["success"] = this->success_;

    return json_object;
}