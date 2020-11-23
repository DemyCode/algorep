#include "controller.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "mpi_classes/process-information.hh"
#include "rpc/get-state.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/message.hh"
#include "utils/clock.hh"
#include "utils/string_utils.hh"

Controller::Controller()
    : timeout_(500) // TODO define timeout
    , next_uid_(0)
{}

void Controller::print_help()
{
    std::cout << "Available commands:" << std::endl
              << "- help                              - print this message" << std::endl
              << "- list_ranks                        - print the list of ranks with the type of the process associated"
              << std::endl
              << "- entry [client_rank] [command]     - send an entry to the client_rank" << std::endl
              << "- start [client_rank]               - start the client_rank" << std::endl
              << "- set_speed [process_rank] [speed]  - set the speed of the process_rank, speed available: {low, "
                 "medium, high}"
              << std::endl
              << "- crash [process_rank]              - crash the process_rank" << std::endl
              << "- recover [process_rank]            - recover the process_rank" << std::endl
              << "- stop [process_rank]               - stop the process_rank" << std::endl
              << "- auto_stop [client_rank]           - stop the client_rank when his queue is empty and send message "
                 "to all server to stop"
              << std::endl
              << "- get_state [server_rank]           - get the state of the server_rank" << std::endl
              << "- sleep [milliseconds]              - sleep the current process for milliseconds" << std::endl;
}

void Controller::list_ranks()
{
    int max_size = std::to_string(ProcessInformation::instance().size_ - 1).size();
    std::string separator(17 + max_size, '-');

    std::cout << std::setfill(' ') << "Ranks:" << std::endl
              << separator << std::endl
              << "| " << std::setw(max_size) << ProcessInformation::instance().rank_ << " | controller |" << std::endl;

    for (int client_rank = ProcessInformation::instance().client_offset_;
         client_rank < ProcessInformation::instance().client_offset_ + ProcessInformation::instance().n_client_;
         client_rank++)
        std::cout << "| " << std::setw(max_size) << client_rank << " |   client   |" << std::endl;

    for (int server_rank = ProcessInformation::instance().node_offset_;
         server_rank < ProcessInformation::instance().node_offset_ + ProcessInformation::instance().n_node_;
         server_rank++)
        std::cout << "| " << std::setw(max_size) << server_rank << " |   server   |" << std::endl;

    std::cout << separator << std::endl << std::resetiosflags(std::ios::showbase);
}

void Controller::send_message(int destination_rank,
                              const Message::MESSAGE_TYPE& message_type,
                              const std::string& command)
{
    Message message(this->next_uid_++, message_type, command);
    ::send_message(message, destination_rank);
}

void Controller::handle_entry(const std::vector<std::string>& tokens, const std::string& line)
{
    if (tokens.size() <= 2)
    {
        std::cout << "Invalid arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    std::vector<std::string> sub_tokens;
    auto position = split_string(line, " ", sub_tokens, 2);
    position = line.find_first_not_of(' ', position);

    std::string command = line.substr(position, line.size() - position);

    this->send_message(destination_rank, Message::MESSAGE_TYPE::CLIENT_CREATE_NEW_ENTRY, command);
}

void Controller::handle_set_speed(const std::vector<std::string>& tokens)
{
    if (tokens.size() != 3)
    {
        std::cout << "Invalid arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank)
        && !ProcessInformation::instance().is_rank_node(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    const auto& speed = tokens[2];
    if (speed != "low" && speed != "medium" && speed != "high")
    {
        std::cout << "Invalid speed: " << tokens[2] << std::endl;
        return;
    }

    this->send_message(destination_rank, Message::MESSAGE_TYPE::PROCESS_SET_SPEED, speed);
}

void Controller::handle_crash(const std::vector<std::string>& tokens)
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank)
        && !ProcessInformation::instance().is_rank_node(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    this->send_message(destination_rank, Message::MESSAGE_TYPE::PROCESS_CRASH);
}

void Controller::handle_start(const std::vector<std::string>& tokens)
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    this->send_message(destination_rank, Message::MESSAGE_TYPE::CLIENT_START);
}

void Controller::handle_recover(const std::vector<std::string>& tokens)
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank)
        && !ProcessInformation::instance().is_rank_node(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    this->send_message(destination_rank, Message::MESSAGE_TYPE::PROCESS_RECOVER);
}

void Controller::handle_stop(const std::vector<std::string>& tokens)
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank)
        && !ProcessInformation::instance().is_rank_node(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    this->send_message(destination_rank, Message::MESSAGE_TYPE::PROCESS_STOP);
}

void Controller::handle_auto_stop(const std::vector<std::string>& tokens)
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_client(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    this->send_message(destination_rank, Message::MESSAGE_TYPE::CLIENT_AUTO_STOP);
}

void Controller::handle_get_state(const std::vector<std::string>& tokens) const
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int destination_rank = parse_int(tokens[1]);
    if (!ProcessInformation::instance().is_rank_node(destination_rank))
    {
        std::cout << "Invalid rank: " << tokens[1] << std::endl;
        return;
    }

    GetState get_state(destination_rank);
    ::send_message(get_state, destination_rank);

    auto query = wait_message(destination_rank, this->timeout_);

    std::string status = "DEAD";

    if (query.has_value())
    {
        auto get_state_response = std::get<GetStateResponse>(query->content_);
        switch (get_state_response.state_)
        {
            case GetStateResponse::STATE::LEADER:
                status = "LEADER";
                break;

            case GetStateResponse::STATE::FOLLOWER:
                status = "FOLLOWER";
                break;

            case GetStateResponse::STATE::CANDIDATE:
                status = "CANDIDATE";
                break;

            case GetStateResponse::STATE::STOPPED:
                status = "STOPPED";
                break;
        }
    }

    std::cout << "Status " << destination_rank << ": " << status << std::endl;
}

void Controller::handle_sleep(const std::vector<std::string>& tokens)
{
    if (tokens.size() <= 1)
    {
        std::cout << "Missing arguments" << std::endl;
        return;
    }

    int milliseconds = parse_int(tokens[1]);
    if (milliseconds < 0)
    {
        std::cout << "Invalid milliseconds, must be a positive integer: " << tokens[1] << std::endl;
        return;
    }

    Clock::wait(milliseconds);
}

void Controller::run()
{
    std::cout << "> ";

    for (std::string line; std::getline(std::cin, line);)
    {
        std::vector<std::string> tokens;
        split_string(line, " ", tokens);

        if (!tokens.empty())
        {
            if (tokens[0] == "help")
                print_help();
            else if (tokens[0] == "list_ranks")
                list_ranks();
            else if (tokens[0] == "entry")
                this->handle_entry(tokens, line);
            else if (tokens[0] == "set_speed")
                this->handle_set_speed(tokens);
            else if (tokens[0] == "crash")
                this->handle_crash(tokens);
            else if (tokens[0] == "start")
                this->handle_start(tokens);
            else if (tokens[0] == "recover")
                this->handle_recover(tokens);
            else if (tokens[0] == "stop")
                this->handle_stop(tokens);
            else if (tokens[0] == "auto_stop")
                this->handle_auto_stop(tokens);
            else if (tokens[0] == "get_state")
                this->handle_get_state(tokens);
            else if (tokens[0] == "sleep")
                handle_sleep(tokens);
            else
            {
                std::cout << "Command not found: " << tokens[0] << std::endl;
                print_help();
            }
        }

        std::cout << "> ";
    }
}
