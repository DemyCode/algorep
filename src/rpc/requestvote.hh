//
// Created by mehdi on 19/11/2020.
//

#ifndef ALGOREP_REQUESTVOTE_HH
#define ALGOREP_REQUESTVOTE_HH

#include <string>
#include <json.hpp>

using json = nlohmann::json;

class RequestVote {
    RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm);
    RequestVote(std::string serialized);
    std::string serialize();

    int term;
    int candidateId;
    int lastLogIndex;
    int lastLogTerm;
};


#endif //ALGOREP_REQUESTVOTE_HH
