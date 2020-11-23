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
    void send_entries();
    void check_timeouts();
    void reset_leader();

    const float timeout_;

    Message::SPEED_TYPE speed_;

    bool start_;
    bool stop_;

    int leader_;
    Clock leader_search_clock_;

    int next_uid_;

    std::queue<NewEntry> entries_to_send_;
    std::unordered_map<int, NewEntry> entries_;
    std::unordered_map<int, Clock> entries_clocks_;
    std::unordered_map<int, int> entries_leaders_;
};
