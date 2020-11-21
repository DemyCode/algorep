#pragma once

#include "rpc.hh"

class NewEntryResponse : public RPC
{
public:
    using json = nlohmann::json;

    NewEntryResponse(int uid, bool success);
    NewEntryResponse(const json& serialized_json);
    NewEntryResponse(const std::string& serialized);

    json serialize_message() const override;

    const int uid_;
    const bool success_;
};