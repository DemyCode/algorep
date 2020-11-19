#pragma once

#include <string>

#include "json/json.hpp"

class AppendEntriesResponse
{
public:
    using json = nlohmann::json;

    AppendEntriesResponse(int term, bool success);
    AppendEntriesResponse(const json& serialized_json);
    AppendEntriesResponse(const std::string& serialized);

    std::string serialized() const;

private:
    // current_term, for leader to update itself
    const int term_;
    // true if follower contained entry matching prev_log_index and prev_log_term
    const bool success_;
};
