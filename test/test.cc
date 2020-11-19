//
// Created by mehdi on 19/11/2020.
//

#include <iostream>
#include <vector>

class entry
{
public:
    int term;
    std::string command;
};

class AppendEntriesStruct {
public:
    int leaderId;
    int prevLogIndex;
    int prevLogTerm;
    std::vector<entry> entries;
    int leaderCommit;
};

int determine_size(AppendEntriesStruct aStruct)
{
    int result = 0;
    for (size_t i = 0; i < aStruct.entries.size(); i++)
    {
        result += aStruct.entries[i].command.size() * sizeof(char);
        result += sizeof(entry);
    }
    return 0;
}

int main()
{
    std::string yes = "yes";
    std::cout << sizeof(char) * yes.size() << std::endl;
    std::string ok;
    return 0;
}