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
    static void list_ranks();
    void
    send_message(int destination_rank, const Message::MESSAGE_TYPE& message_type, const std::string& command = "empty");
    void handle_entry(const std::vector<std::string>& tokens, const std::string& line);
    void handle_set_speed(const std::vector<std::string>& tokens);
    void handle_crash(const std::vector<std::string>& tokens);
    void handle_start(const std::vector<std::string>& tokens);
    void handle_recover(const std::vector<std::string>& tokens);
    void handle_stop(const std::vector<std::string>& tokens);
    void handle_auto_stop(const std::vector<std::string>& tokens);
    void handle_get_state(const std::vector<std::string>& tokens) const;
    static void handle_sleep(const std::vector<std::string>& tokens);

    const float timeout_;
    int next_uid_;
};