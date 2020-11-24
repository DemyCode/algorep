#pragma once

#include <cstddef>

#include "utils/singleton.hh"

class ProcessInformation : public Singleton<ProcessInformation>
{
    friend class Singleton<ProcessInformation>;

public:
    void
    set_information(size_t rank, size_t n_client, size_t client_offset, size_t n_node, size_t node_offset, size_t size);
    bool is_rank_client(size_t rank) const;
    bool is_rank_node(size_t rank) const;

    size_t rank_;
    size_t n_client_;
    size_t client_offset_;
    size_t n_node_;
    size_t node_offset_;
    size_t size_;

private:
    ProcessInformation() = default;
};
