#pragma once

#include "utils/singleton.hh"

class ProcessInformation : public Singleton<ProcessInformation>
{
    friend class Singleton<ProcessInformation>;

public:
    void set_information(int rank, int n_client, int client_offset, int n_node, int node_offset, int size);
    bool is_rank_client(int rank) const;
    bool is_rank_node(int rank) const;

    int rank_;
    int n_client_;
    int client_offset_;
    int n_node_;
    int node_offset_;
    int size_;

private:
    ProcessInformation() = default;
};
