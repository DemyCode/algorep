#include "append-entries-response.hh"

AppendEntriesResponse::AppendEntriesResponse(int term, bool success)
    : term_(term)
    , success_(success)
{}

AppendEntriesResponse::AppendEntriesResponse(const json& serialized_json)
    : term_(serialized_json["term"])
    , success_(serialized_json["success"])
{}

AppendEntriesResponse::AppendEntriesResponse(const std::string& serialized)
    : AppendEntriesResponse(json::parse(serialized))
{}

std::string AppendEntriesResponse::serialized() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["success"] = this->success_;

    return json_object.dump();
}