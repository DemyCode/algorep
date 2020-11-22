#include "process-information.hh"

void ProcessInformation::set_information(int rank,
                                         int n_client,
                                         int client_offset,
                                         int n_node,
                                         int node_offset,
                                         int size)
{
    this->rank_ = rank;
    this->n_client_ = n_client;
    this->client_offset_ = client_offset;
    this->n_node_ = n_node;
    this->node_offset_ = node_offset;
    this->size_ = size;
}

bool ProcessInformation::is_rank_client(int rank) const
{
    return rank >= this->client_offset_ && rank < this->client_offset_ + this->n_client_;
}

bool ProcessInformation::is_rank_node(int rank) const
{
    return rank >= this->node_offset_ && rank < this->node_offset_ + this->n_node_;
}