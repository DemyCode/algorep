//
// Created by mehdi on 19/11/2020.
//

#include "requestvoteresponse.hh"

RequestVoteResponse::RequestVoteResponse(int term, bool voteGranted)
{
    this->term = term;
    this->voteGranted = voteGranted;
}

RequestVoteResponse::RequestVoteResponse(std::string serialized)
{
    json j = json::parse(serialized);
    this->term = j["term"];
    this->voteGranted = j["voteGranted"];
}

std::string RequestVoteResponse::serialized()
{
    json j;
    j["term"] = this->term;
    j["voteGranted"] = this->voteGranted;
    return j.dump();
}