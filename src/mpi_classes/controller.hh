#pragma once

#include <string>

#include "rpc/message.hh"

class Controller
{
public:
    Controller();

    void run();

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
    void handle_stop(const std::vector<std::string>& tokens);

    int next_uid_;
};