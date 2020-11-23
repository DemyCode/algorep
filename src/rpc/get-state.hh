#pragma once

#include "rpc.hh"

class GetState : public RPC
{
public:
    using json = nlohmann::json;

    GetState(int process_rank);
    GetState(const json& serialized_json);
    GetState(const std::string& serialized);

    json serialize_message() const override;

    const int process_rank_;
};