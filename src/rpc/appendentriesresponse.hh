//
// Created by mehdi on 19/11/2020.
//

#ifndef ALGOREP_APPENDENTRIESRESPONSE_HH
#define ALGOREP_APPENDENTRIESRESPONSE_HH


class AppendEntriesResponse {
    AppendEntriesResponse(int term, bool success);
    AppendEntriesResponse(std::string serialized);
    std::string serialized();

    int term;
    bool voteGranted;
};


#endif //ALGOREP_APPENDENTRIESRESPONSE_HH
