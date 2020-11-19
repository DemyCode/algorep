//
// Created by mehdi on 19/11/2020.
//

#ifndef ALGOREP_ENTRY_HH
#define ALGOREP_ENTRY_HH

#include <json.hpp>
#include <string>

using json = nlohmann::json;

class Entry {
public:
    Entry(int term, std::string command);
    Entry(std::string serialized);
    std::string serialize();

    int term;
    std::string command;
};


#endif //ALGOREP_ENTRY_HH
