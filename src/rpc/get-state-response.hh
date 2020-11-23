#pragma once

#include "mpi_classes/node.hh"
#include "rpc.hh"

class GetStateResponse : public RPC
{
public:
    using json = nlohmann::json;

    GetStateResponse(Node::state state);
    GetStateResponse(const json& serialized_json);
    GetStateResponse(const std::string& serialized);

    json serialize_message() const override;

    const Node::state state_;
};