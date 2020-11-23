#pragma once

#include "rpc.hh"
#include "utils/entry.hh"

class NewEntry : public RPC
{
public:
    NewEntry(int uid, Entry entry);
    NewEntry(const json& serialized_json);
    NewEntry(const std::string& serialized);

    json serialize_message() const override;

    const int uid_;
    Entry entry_;
};

class NewEntryResponse : public RPC
{
public:
    NewEntryResponse(int uid, bool success);
    NewEntryResponse(const json& serialized_json);
    NewEntryResponse(const std::string& serialized);

    json serialize_message() const override;

    const int uid_;
    const bool success_;
};