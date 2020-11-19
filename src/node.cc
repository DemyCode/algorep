//
// Created by mehdi on 18/11/2020.
//

#include "node.hh"

Node::Node(int rank, int n_node, int offset, int size) {
    this->rank = rank;
    this->n_node = n_node;
    this->offset = offset;
    this->size = size;
    this->state_ = state::FOLLOWER;
    this->term = 0;
}

void Node::run() {
    return;
}

void Node::candidate() {
    /*this->term += 1;
    int vote_count = 1;
    std::vector<MPI_Request> requests = std::vector<MPI_Request>();
    for (int i = offset; i < offset + n_node; i++)
    {
        MPI_Isend(&voting, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &requests[i]);
    }
    // sleep 0 - 150 ms
    for (int i = offset; i < offset + n_node; i++)
    {
        MPI_Irecv()
    }*/
}
