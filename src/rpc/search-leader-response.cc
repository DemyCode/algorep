#include "search-leader-response.hh"

SearchLeaderResponse::SearchLeaderResponse(int new_leader)
    : RPC(-1, RPC::RPC_TYPE::SEARCH_LEADER_RESPONSE)
    , new_leader_(new_leader)
{}

SearchLeaderResponse::SearchLeaderResponse(const json& serialized_json)
    : SearchLeaderResponse(serialized_json["new_leader"].get<int>())
{}

SearchLeaderResponse::SearchLeaderResponse(const std::string& serialized)
    : SearchLeaderResponse(json::parse(serialized))
{}

SearchLeaderResponse::json SearchLeaderResponse::serialize_message() const
{
    json json_object;

    json_object["new_leader"] = this->new_leader_;

    return json_object;
}