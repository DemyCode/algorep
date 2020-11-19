//
// Created by mehdi on 19/11/2020.
//

#ifndef ALGOREP_REQUESTVOTERESPONSE_HH
#define ALGOREP_REQUESTVOTERESPONSE_HH

#include <json.hpp>

using json = nlohmann::json;

class RequestVoteResponse {
    RequestVoteResponse(int term, bool success);
    RequestVoteResponse(std::string serialized);
    std::string serialized();

    int term;
    bool voteGranted;
};


#endif //ALGOREP_REQUESTVOTERESPONSE_HH
