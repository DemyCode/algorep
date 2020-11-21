#include "new-entry-response.hh"

NewEntryResponse::NewEntryResponse(int uid, bool success)
    : RPC(-1, RPC::RPC_TYPE::NEW_ENTRY_RESPONSE)
    , uid_(uid)
    , success_(success)
{}

NewEntryResponse::NewEntryResponse(const json& serialized_json)
    : NewEntryResponse(serialized_json["uid"], serialized_json["success"])
{}

NewEntryResponse::NewEntryResponse(const std::string& serialized)
    : NewEntryResponse(json::parse(serialized))
{}

NewEntryResponse::json NewEntryResponse::serialize_message() const
{
    json json_object;

    json_object["uid"] = this->uid_;
    json_object["success"] = this->success_;

    return json_object;
}