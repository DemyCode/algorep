#pragma once

#include <json.hpp>
#include <rpc/entry.hh>
#include <string>
#include <vector>

using json = nlohmann::json;

class AppendEntries
{
public:
    AppendEntries(int term,
                  int leaderId,
                  int preLogIndex,
                  int prevLogTerm,
                  std::vector<Entry> entries,
                  int leaderCommit);
    AppendEntries(std::string serialized);
    std::string serialize();

    int term;
    int leaderId;
    int prevLogIndex;
    int prevLogTerm;
    std::vector<Entry> entries;
    int leaderCommit;
};
