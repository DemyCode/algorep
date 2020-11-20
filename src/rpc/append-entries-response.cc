#include "append-entries-response.hh"

AppendEntriesResponse::AppendEntriesResponse(int term, bool success)
    : RPC(RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
    , term_(term)
    , success_(success)
{}

AppendEntriesResponse::AppendEntriesResponse(const json& serialized_json)
    : AppendEntriesResponse(serialized_json["term"], serialized_json["success"])
{}

AppendEntriesResponse::AppendEntriesResponse(const std::string& serialized)
    : AppendEntriesResponse(json::parse(serialized))
{}

AppendEntriesResponse::json AppendEntriesResponse::serialize_message() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["success"] = this->success_;

    return json_object;
}