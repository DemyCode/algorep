#include "entry.hh"

Entry::Entry(int term, std::string command)
    : RPC(RPC::RPC_TYPE::ENTRY)
    , term_(term)
    , command_(command)
{}

Entry::Entry(const json& serialized_json)
    : Entry(serialized_json["term"], serialized_json["command"])
{}

Entry::Entry(const std::string& serialized)
    : Entry(json::parse(serialized))
{}

Entry::json Entry::serialize_message() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["command"] = this->command_;

    return json_object;
}
