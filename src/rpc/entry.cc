//
// Created by mehdi on 19/11/2020.
//


#include "entry.hh"

Entry::Entry(int term, std::string command) {
    this->term = term;
    this->command = command;
}

Entry::Entry(std::string serialized) {
    json j = json::parse(serialized);
    this->term = j["term"];
    this->command = j["command"];
}

std::string Entry::serialize() {
    json j;
    j["term"] = this->term;
    j["command"] = this->command;
    return j.dump();
}
