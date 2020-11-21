#include "new-entry-response.hh"

NewEntryResponse::NewEntryResponse(bool success)
    : RPC(-1, RPC::RPC_TYPE::NEW_ENTRY_RESPONSE)
    , success_(success)
{}

NewEntryResponse::NewEntryResponse(const json& serialized_json)
    : NewEntryResponse(serialized_json["success"].get<bool>())
{}

NewEntryResponse::NewEntryResponse(const std::string& serialized)
    : NewEntryResponse(json::parse(serialized))
{}

NewEntryResponse::json NewEntryResponse::serialize_message() const
{
    json json_object;

    json_object["success"] = this->success_;

    return json_object;
}