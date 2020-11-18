//
// Created by mehdi on 18/11/2020.
//

#ifndef ALGOREP_NODE_HH
#define ALGOREP_NODE_HH

enum state {
    FOLLOWER, CANDIDATE, LEADER, STOPPED
};

class Node {
public:
    Node(int rank, int n_node, int offset, int size);
    void run();
    void candidate();
private:
    state state;
    int rank;
    int n_node;
    int offset;
    int size;
    int term;
    float election_timeout; // 150 - 300 ms
    float heart_beat; // 0 - 150 ms
};


#endif //ALGOREP_NODE_HH
