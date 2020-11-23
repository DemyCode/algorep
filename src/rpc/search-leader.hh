#pragma once

#include "rpc.hh"

class SearchLeader : public RPC
{
public:
    SearchLeader(int old_leader);
    SearchLeader(const json& serialized_json);
    SearchLeader(const std::string& serialized);

    json serialize_message() const override;

    const int old_leader_;
};

class SearchLeaderResponse : public RPC
{
public:
    SearchLeaderResponse(int new_leader);
    SearchLeaderResponse(const json& serialized_json);
    SearchLeaderResponse(const std::string& serialized);

    json serialize_message() const override;

    const int new_leader_;
};