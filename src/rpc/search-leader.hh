#pragma once

#include "rpc.hh"

class SearchLeader : public RPC
{
public:
    using json = nlohmann::json;

    SearchLeader(int old_leader);
    SearchLeader(const json& serialized_json);
    SearchLeader(const std::string& serialized);

    json serialize_message() const override;

    const int old_leader_;
};
