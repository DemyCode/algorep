#include "new-entry.hh"

#include <utility>

NewEntry::NewEntry(Entry entry)
    : RPC(-1, RPC::RPC_TYPE::NEW_ENTRY)
    , entry_(std::move(entry))
{}

NewEntry::NewEntry(const json& serialized_json)
    : NewEntry(Entry(serialized_json["entry"]))
{}

NewEntry::NewEntry(const std::string& serialized)
    : NewEntry(json::parse(serialized))
{}

RPC::json NewEntry::serialize_message() const
{
    json json_object;

    json_object["entry"] = this->entry_.serialize_message();

    return json_object;
}

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

RPC::json NewEntryResponse::serialize_message() const
{
    json json_object;

    json_object["success"] = this->success_;

    return json_object;
}