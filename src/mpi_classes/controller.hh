#pragma once

#include <string>

#include "rpc/message.hh"

class Controller
{
public:
    Controller(int rank, int n_client, int client_offset, int n_node, int node_offset);

    void start_controller();

private:
    static void print_help();
    void list_ranks() const;
    void
    send_message(int destination_rank, const Message::MESSAGE_TYPE& message_type, const std::string& command = "empty");
    void handle_entry(const std::vector<std::string>& tokens, const std::string& line);
    void handle_set_speed(const std::vector<std::string>& tokens);
    void handle_crash(const std::vector<std::string>& tokens);
    void handle_start(const std::vector<std::string>& tokens);
    void handle_recover(const std::vector<std::string>& tokens);

    const int rank_;
    const int n_client_;
    const int client_offset_;
    const int n_node_;
    const int node_offset_;

    int next_uid_;
};