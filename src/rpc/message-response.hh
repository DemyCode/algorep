#pragma once

#include "rpc.hh"

class MessageResponse : public RPC
{
public:
    using json = nlohmann::json;

    MessageResponse(int uid, bool success);
    MessageResponse(const json& serialized_json);
    MessageResponse(const std::string& serialized);

    json serialize_message() const override;

    const int uid_;
    const bool success_;
};