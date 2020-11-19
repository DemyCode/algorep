//
// Created by mehdi on 19/11/2020.
//

#include "appendentriesresponse.hh"

AppendEntriesResponse::AppendEntriesResponse(int term, bool success) {
    this->term = term;
    this->success = success;
}

AppendEntriesResponse::AppendEntriesResponse(std::string serialized) {
    json j = json::parse(serialized);
    this->term = j["term"];
    this->success = j["success"];
}

std::string AppendEntriesResponse::serialized() {
    json j;
    j["term"] = this->term;
    j["success"] = this->success;
    return j.dump();
}