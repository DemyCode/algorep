#pragma once

#include <string>

class Controller
{
public:
    Controller(int rank, int n_client, int client_offset, int n_node, int node_offset);

    void start_controller();

private:
    void list_ranks();
    static void print_help();
    void send_entry(int destination_rank, const std::string& command);

    const int rank_;
    const int n_client_;
    const int client_offset_;
    const int n_node_;
    const int node_offset_;

    int next_uid_;
};