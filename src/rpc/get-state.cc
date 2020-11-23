#include "get-state.hh"

GetState::GetState(int process_rank)
    : RPC(-1, RPC::RPC_TYPE::GET_STATE)
    , process_rank_(process_rank)
{}

GetState::GetState(const json& serialized_json)
    : GetState(serialized_json["process_rank"].get<int>())
{}

GetState::GetState(const std::string& serialized)
    : GetState(json::parse(serialized))
{}

GetState::json GetState::serialize_message() const
{
    json json_object;

    json_object["process_rank"] = this->process_rank_;

    return json_object;
}