#pragma once

#include <string>
#include <vector>

#include "json/json.hpp"
#include "rpc.hh"
#include "rpc/entry.hh"

class AppendEntries : public RPC
{
public:
    using json = nlohmann::json;

    AppendEntries(int term,
                  int leader_id,
                  int prev_log_index,
                  int prev_log_term,
                  std::vector<Entry> entries,
                  int leader_commit);
    AppendEntries(const json& serialized_json);
    AppendEntries(const std::string& serialized);

    json serialize_message() const override;

private:
    // leader's term
    const int term_;
    // so follower can redirect clients
    const int leader_id_;
    // index of log entry immediately preceding new ones
    const int prev_log_index_;
    // term of prev_log_index entry
    const int prev_log_term_;
    // log entries to store (empty for heartbeat may send more than one for efficiency)
    std::vector<Entry> entries_;
    // leader's commit_index
    const int leader_commit_;
};
