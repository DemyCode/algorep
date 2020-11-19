#include "requestvote.hh"

RequestVote::RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm)
{
    this->term = term;
    this->candidateId = candidateId;
    this->lastLogIndex = lastLogIndex;
    this->lastLogTerm = lastLogTerm;
}

RequestVote::RequestVote(std::string serialized)
{
    json j = json::parse(serialized);
    j["term"] = this->term;
    j["candidateId"] = this->candidateId;
    j["lastLogIndex"] = this->lastLogIndex;
    j["lastLogTerm"] = this->lastLogTerm;
}

std::string RequestVote::serialize()
{
    json j;
    this->term = j["term"];
    this->candidateId = j["candidateId"];
    this->lastLogIndex = j["lastLogIndex"];
    this->lastLogTerm = j["lastLogTerm"];
    return j.dump();
}
