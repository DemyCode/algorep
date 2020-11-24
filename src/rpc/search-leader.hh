#pragma once

#include "rpc.hh"

class SearchLeader : public RPC
{
public:
    SearchLeader(size_t old_leader);
    SearchLeader(const json& serialized_json);
    SearchLeader(const std::string& serialized);

    json serialize_message() const override;

    const size_t old_leader_;
};

class SearchLeaderResponse : public RPC
{
public:
    SearchLeaderResponse(size_t new_leader);
    SearchLeaderResponse(const json& serialized_json);
    SearchLeaderResponse(const std::string& serialized);

    json serialize_message() const override;

    const size_t new_leader_;
};