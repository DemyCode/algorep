//
// Created by mehdi on 18/11/2020.
//

#ifndef ALGOREP_NODE_HH
#define ALGOREP_NODE_HH

#include <mpi.h>
#include <vector>
#include <chrono>
#include <string>
#include <optional>
#include <iostream>

enum state {
    FOLLOWER, CANDIDATE, LEADER, STOPPED
};

enum ping {
    REQUESTVOTE, VOTE
};

class Node {
public:
    Node(int rank, int n_node, int offset, int size);
    void run();
    void follower_run();
    void leader_run();
    void candidate_run();
private:
    // MPI VALUES
    int rank_;
    int n_node_;
    int offset_;
    int size_;
    state state_;
    float election_timeout;

    // RAFT VALUES
    // Persistent on all servers
    int currentTerm;
    int votedFor;
    std::vector<std::string> log;
    // Volatile on all servers
    int commitIndex;
    int lastApplied;
    // Volatile state for leader
    std::vector<int> nextIndex;
    std::vector<int> matchIndex;
};


#endif //ALGOREP_NODE_HH
