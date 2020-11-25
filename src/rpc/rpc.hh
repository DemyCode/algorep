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

    // term of the RPC
    const int term_;
    // type of the RPC

    ///Type of the rpc
    const RPC_TYPE rpc_type_;
};
