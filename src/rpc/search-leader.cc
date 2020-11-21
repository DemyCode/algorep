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

SearchLeader::json SearchLeader::serialize_message() const
{
    json json_object;

    json_object["old_leader"] = this->old_leader_;

    return json_object;
}