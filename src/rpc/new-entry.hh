#pragma once

#include "rpc.hh"
#include "utils/entry.hh"

class NewEntry : public RPC
{
public:
    using json = nlohmann::json;

    NewEntry(Entry entry);
    NewEntry(const json& serialized_json);
    NewEntry(const std::string& serialized);

    json serialize_message() const override;

    Entry entry_;
};