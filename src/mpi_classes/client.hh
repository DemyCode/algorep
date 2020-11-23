#pragma once

#include <map>
#include <queue>

#include "rpc/rpc-query.hh"
#include "utils/clock.hh"

class Client
{
public:
    Client();

    void run();
    void add_command_list(const std::string& command_list_path);

private:
    void handle_queries(const std::vector<RPCQuery>& queries);
    void handle_message(const RPCQuery& query);
    void handle_search_leader_response(const RPCQuery& query);
    void handle_new_entry_response(const RPCQuery& query);
    void search_leader();
    void send_next_entry();
    void check_timeout();
    void reset_leader();
    static void stop_nodes();

    const float timeout_;

    Message::SPEED_TYPE speed_;

    bool start_;
    bool stop_;
    bool auto_stop_;

    int leader_;
    Clock leader_search_clock_;

    std::queue<NewEntry> entries_to_send_;

    bool entry_sent_;
    Clock entry_clock_;
};
