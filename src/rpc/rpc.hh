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

    ///Constructor
    RPC(int term, RPC_TYPE rpc_type);

    ///Destructor
    virtual ~RPC() = default;

    /**
     * @brief Serialize the rpc in a string
     * 
     * @return std::string 
     */
    std::string serialize() const;

    /**
     * @brief Serialize the rpc in a json
     * 
     * @return json 
     */
    virtual json serialize_message() const = 0;

    ///Term of the rpc
    const int term_;

    ///Type of the rpc
    const RPC_TYPE rpc_type_;
};
