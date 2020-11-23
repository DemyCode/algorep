#pragma once

#include "rpc.hh"
#include "utils/entry.hh"

class NewEntry : public RPC
{
public:
    NewEntry(Entry entry);
    NewEntry(const json& serialized_json);
    NewEntry(const std::string& serialized);

    json serialize_message() const override;

    Entry entry_;
};

class NewEntryResponse : public RPC
{
public:
    NewEntryResponse(bool success);
    NewEntryResponse(const json& serialized_json);
    NewEntryResponse(const std::string& serialized);

    json serialize_message() const override;

    const bool success_;
};