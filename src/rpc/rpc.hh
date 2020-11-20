#pragma once

#include <string>

#include "json/json.hpp"

class RPC
{
public:
    enum RPC_TYPE
    {
        APPEND_ENTRIES,
        APPEND_ENTRIES_RESPONSE,
        ENTRY,
        REQUEST_VOTE,
        REQUEST_VOTE_RESPONSE
    };

    using json = nlohmann::json;

    RPC(RPC_TYPE rpc_type);
    virtual ~RPC() = default;

    std::string serialize() const;
    virtual json serialize_message() const = 0;

private:
    const RPC_TYPE rpc_type_;
};
