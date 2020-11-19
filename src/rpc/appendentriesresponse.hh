//
// Created by mehdi on 19/11/2020.
//

#ifndef ALGOREP_APPENDENTRIESRESPONSE_HH
#define ALGOREP_APPENDENTRIESRESPONSE_HH

#include <string>
#include <json.hpp>

using json = nlohmann::json;

class AppendEntriesResponse {
    AppendEntriesResponse(int term, bool success);
    AppendEntriesResponse(std::string serialized);
    std::string serialized();

    int term;
    bool success;
};


#endif //ALGOREP_APPENDENTRIESRESPONSE_HH
