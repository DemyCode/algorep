#pragma once

#include <string>
#include <vector>

#include "json/json.hpp"
#include "rpc.hh"
#include "utils/entry.hh"

class AppendEntries : public RPC
{
public:
    AppendEntries(int term,
                  int leader_id,
                  int prev_log_index,
                  int prev_log_term,
                  std::vector<Entry> entries,
                  int leader_commit);
    AppendEntries(int term, const json& serialized_json);
    AppendEntries(int term, const std::string& serialized);

    json serialize_message() const override;

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

class AppendEntriesResponse : public RPC
{
public:
    AppendEntriesResponse(int term, bool success);
    AppendEntriesResponse(int term, const json& serialized_json);
    AppendEntriesResponse(int term, const std::string& serialized);

    json serialize_message() const override;

    // true if follower contained entry matching prev_log_index and prev_log_term
    const bool success_;
};
