#pragma once

#include "rpc.hh"

class MessageResponse : public RPC
{
public:
    using json = nlohmann::json;

    MessageResponse(bool success);
    MessageResponse(const json& serialized_json);
    MessageResponse(const std::string& serialized);

    json serialize_message() const override;

    const bool success_;
};