#pragma once

#include "rpc.hh"

class SearchLeaderResponse : public RPC
{
public:
    using json = nlohmann::json;

    SearchLeaderResponse(int new_leader);
    SearchLeaderResponse(const json& serialized_json);
    SearchLeaderResponse(const std::string& serialized);

    json serialize_message() const override;

    const int new_leader_;
};