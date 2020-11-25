#include "entry.hh"

#include <utility>

Entry::Entry(int term, std::string command)
    : term_(term)
    , command_(std::move(command))
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
