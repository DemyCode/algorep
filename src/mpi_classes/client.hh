#pragma once

#include <map>
#include <queue>

#include "rpc/rpc-query.hh"
#include "utils/clock.hh"

class Client
{
public:
    Client(int rank, int n_client, int client_offset, int n_node, int node_offset, int size);

    void run();

private:
    void handle_queries(const std::vector<RPCQuery>& queries);
    void handle_message(const RPCQuery& query);
    void handle_search_leader_response(const RPCQuery& query);
    void handle_new_entry_response(const RPCQuery& query);
    void search_leader() const;
    void send_entries();
    void check_timeouts();

    const int rank_;
    const int n_client_;
    const int client_offset_;
    const int n_node_;
    const int node_offset_;
    const int size_;
    const float timeout_;

    int leader_;

    int next_uid_;

    std::queue<NewEntry> entries_to_send_;
    std::unordered_map<int, NewEntry> entries_;
    std::unordered_map<int, Clock> entries_clocks_;
    std::unordered_map<int, int> entries_leaders_;
};
