#include "search-leader.hh"

SearchLeader::SearchLeader(int old_leader)
    : RPC(-1, RPC::RPC_TYPE::SEARCH_LEADER)
    , old_leader_(old_leader)
{}

SearchLeader::SearchLeader(const json& serialized_json)
    : SearchLeader(serialized_json["old_leader"].get<int>())
{}

SearchLeader::SearchLeader(const std::string& serialized)
    : SearchLeader(json::parse(serialized))
{}

RPC::json SearchLeader::serialize_message() const
{
    json json_object;

    json_object["old_leader"] = this->old_leader_;

    return json_object;
}

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

RPC::json SearchLeaderResponse::serialize_message() const
{
    json json_object;

    json_object["new_leader"] = this->new_leader_;

    return json_object;
}