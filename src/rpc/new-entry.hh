#pragma once

#include "rpc.hh"
#include "utils/entry.hh"

class NewEntry : public RPC
{
public:
    using json = nlohmann::json;

    NewEntry(int uid, Entry entry);
    NewEntry(const json& serialized_json);
    NewEntry(const std::string& serialized);

    json serialize_message() const override;

    const int uid_;
    Entry entry_;
};