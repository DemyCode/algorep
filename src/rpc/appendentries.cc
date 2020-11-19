//
// Created by mehdi on 19/11/2020.
//

#include "appendentries.hh"

AppendEntries::AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm, std::vector<Entry> entries, int leaderCommit)
{
    this->term = term;
    this->leaderId = leaderId;
    this->prevLogIndex = prevLogIndex;
    this->prevLogTerm = prevLogTerm;
    this->entries = entries;
    this->leaderCommit = leaderCommit;
}

AppendEntries::AppendEntries(std::string serialized) {
    json j = json::parse(serialized);
    this->term = j["term"];
    this->leaderId = j["leaderId"];
    this->prevLogIndex = j["prevLogIndex"];
    this->prevLogTerm = j["prevLogTerm"];
    for (auto & entry : entries)
        entry = Entry(entry.term, entry.command);
    this->leaderCommit = j["leaderCommit"];
}

std::string AppendEntries::serialize() {
    json j;
    j["term"] = term;
    j["leaderId"] = leaderId;
    j["prevLogIndex"] = prevLogIndex;
    j["prevLogTerm"] = prevLogTerm;
    for (auto & entry : entries)
        j["entries"].push_back(entry.serialize());
    j["leaderCommit"] = leaderCommit;
    return j.dump();
}
