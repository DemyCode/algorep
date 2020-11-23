#include <utility>

#include "new-entry.hh"
NewEntry::NewEntry(int uid, Entry entry)
    : RPC(-1, RPC::RPC_TYPE::NEW_ENTRY)
    , uid_(uid)
    , entry_(std::move(entry))
{}

NewEntry::NewEntry(const json& serialized_json)
    : NewEntry(serialized_json["uid"], Entry(serialized_json["entry"]))
{}

NewEntry::NewEntry(const std::string& serialized)
    : NewEntry(json::parse(serialized))
{}

RPC::json NewEntry::serialize_message() const
{
    json json_object;

    json_object["uid"] = this->uid_;
    json_object["entry"] = this->entry_.serialize_message();

    return json_object;
}

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

RPC::json NewEntryResponse::serialize_message() const
{
    json json_object;

    json_object["uid"] = this->uid_;
    json_object["success"] = this->success_;

    return json_object;
}