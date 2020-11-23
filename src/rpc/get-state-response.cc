#include "get-state-response.hh"

GetStateResponse::GetStateResponse(Node::state state)
    : RPC(-1, RPC::RPC_TYPE::GET_STATE_RESPONSE)
    , state_(state)
{}

GetStateResponse::GetStateResponse(const json& serialized_json)
    : GetStateResponse(serialized_json["state"].get<Node::state>())
{}

GetStateResponse::GetStateResponse(const std::string& serialized)
    : GetStateResponse(json::parse(serialized))
{}

GetStateResponse::json GetStateResponse::serialize_message() const
{
    json json_object;

    json_object["state"] = this->state_;

    return json_object;
}