#pragma once

#include "rpc.hh"
#include "utils/entry.hh"

class NewEntry : public RPC
{
public:
    /**
     * @brief Entry received from Client, handled by the Leader
     * @param entry content and log of the command
     */
    NewEntry(Entry entry);
    NewEntry(const json& serialized_json);
    NewEntry(const std::string& serialized);

    json serialize_message() const override;

    Entry entry_;
};

class NewEntryResponse : public RPC
{
public:
    /**
     * @brief Server Response from the Leader
     * @param success Response to the Leader if Entry is commited
     */
    NewEntryResponse(bool success);
    NewEntryResponse(const json& serialized_json);
    NewEntryResponse(const std::string& serialized);

    json serialize_message() const override;

    const bool success_;
};