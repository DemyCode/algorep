#include "process-information.hh"

void ProcessInformation::set_information(size_t rank,
                                         size_t n_client,
                                         size_t client_offset,
                                         size_t n_node,
                                         size_t node_offset,
                                         size_t size)
{
    this->rank_ = rank;
    this->n_client_ = n_client;
    this->client_offset_ = client_offset;
    this->n_node_ = n_node;
    this->node_offset_ = node_offset;
    this->size_ = size;
}

bool ProcessInformation::is_rank_client(size_t rank) const
{
    return rank >= this->client_offset_ && rank < this->client_offset_ + this->n_client_;
}

bool ProcessInformation::is_rank_node(size_t rank) const
{
    return rank >= this->node_offset_ && rank < this->node_offset_ + this->n_node_;
}