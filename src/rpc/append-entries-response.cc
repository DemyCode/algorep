#include "append-entries-response.hh"

AppendEntriesResponse::AppendEntriesResponse(int term, bool success)
    : RPC(term, RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
    , success_(success)
{}

AppendEntriesResponse::AppendEntriesResponse(int term, const json& serialized_json)
    : AppendEntriesResponse(term, serialized_json["success"].get<bool>())
{}

AppendEntriesResponse::AppendEntriesResponse(int term, const std::string& serialized)
    : AppendEntriesResponse(term, json::parse(serialized))
{}

AppendEntriesResponse::json AppendEntriesResponse::serialize_message() const
{
    json json_object;

    json_object["success"] = this->success_;

    return json_object;
}