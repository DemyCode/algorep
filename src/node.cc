//
// Created by mehdi on 18/11/2020.
//

#include "node.hh"

Node::Node(int rank, int n_node, int offset, int size) {
    this->rank = rank;
    this->n_node = n_node;
    this->offset = offset;
    this->size = size;
    this->state = state::FOLLOWER;
    this->term = 0;
}

void Node::run() {
    return;
}

void Node::candidate() {
    this->term += 1;
}
