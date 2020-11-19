#include "entry.hh"

Entry::Entry(int term, std::string command)
    : term_(term)
    , command_(command)
{}

Entry::Entry(const json& serialized_json)
    : term_(serialized_json["term"])
    , command_(serialized_json["command"])
{}

Entry::Entry(const std::string& serialized)
    : Entry(json::parse(serialized))
{}

std::string Entry::serialize() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["command"] = this->command_;

    return json_object.dump();
}
