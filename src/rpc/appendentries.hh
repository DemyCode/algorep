//
// Created by mehdi on 19/11/2020.
//

#ifndef ALGOREP_APPENDENTRIES_HH
#define ALGOREP_APPENDENTRIES_HH

#include <vector>
#include <string>
#include <rpc/entry.hh>
#include <json.hpp>

using json = nlohmann::json;

class AppendEntries {
public:
    AppendEntries(int term, int leaderId, int preLogIndex,
                  int prevLogTerm, std::vector<Entry> entries, int leaderCommit);
    AppendEntries(std::string serialized);
    std::string serialize();

    int term;
    int leaderId;
    int prevLogIndex;
    int prevLogTerm;
    std::vector<Entry> entries;
    int leaderCommit;
};


#endif //ALGOREP_APPENDENTRIES_HH
