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

NewEntry::json NewEntry::serialize_message() const
{
    json json_object;

    json_object["uid"] = this->uid_;
    json_object["entry"] = this->entry_.serialize_message();

    return json_object;
}