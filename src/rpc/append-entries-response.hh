#pragma once

#include <string>

#include "json/json.hpp"
#include "rpc.hh"

class AppendEntriesResponse : public RPC
{
public:
    using json = nlohmann::json;

    AppendEntriesResponse(int term, bool success);
    AppendEntriesResponse(const json& serialized_json);
    AppendEntriesResponse(const std::string& serialized);

    json serialize_message() const override;

    // current_term, for leader to update itself
    const int term_;
    // true if follower contained entry matching prev_log_index and prev_log_term
    const bool success_;
};
