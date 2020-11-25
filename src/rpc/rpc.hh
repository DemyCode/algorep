#pragma once

#include <string>

#include "json/json.hpp"

class RPC
{
public:
    /**
    * @brief enum to identify the RPC type
    *
    */
    enum RPC_TYPE
    {
        APPEND_ENTRIES,
        APPEND_ENTRIES_RESPONSE,
        REQUEST_VOTE,
        REQUEST_VOTE_RESPONSE,
        NEW_ENTRY,
        NEW_ENTRY_RESPONSE,
        SEARCH_LEADER,
        SEARCH_LEADER_RESPONSE,
        MESSAGE,
        MESSAGE_RESPONSE,
        GET_STATE,
        GET_STATE_RESPONSE
    };

    using json = nlohmann::json;

    /**
    * @brief Abstract RPC Constructor
    *
    */
    RPC(int term, RPC_TYPE rpc_type);
    virtual ~RPC() = default;

    std::string serialize() const;
    virtual json serialize_message() const = 0;

    // term of the RPC
    const int term_;
    // type of the RPC
    const RPC_TYPE rpc_type_;
};
