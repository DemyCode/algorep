#include "controller.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "rpc/lib-rpc.hh"
#include "rpc/message.hh"
#include "utils/string-utils.hh"

Controller::Controller(int rank, int n_client, int client_offset, int n_node, int node_offset)
    : rank_(rank)
    , n_client_(n_client)
    , client_offset_(client_offset)
    , n_node_(n_node)
    , node_offset_(node_offset)
    , next_uid_(0)
{}

void Controller::print_help()
{
    std::cout << "Available commands:" << std::endl
              << "- help: print this message" << std::endl
              << "- list_ranks: print the list of ranks with the type of the process associated" << std::endl
              << "- entry [client_rank] [command]: send an entry to the client_rank" << std::endl;
}

void Controller::list_ranks()
{
    int max_size = std::to_string(this->node_offset_ + this->n_node_).size();
    std::string separator(17 + max_size, '-');

    std::cout << std::setfill(' ') << separator << std::endl
              << "| " << std::setw(max_size) << this->rank_ << " | controller |" << std::endl;

    for (int client_rank = this->client_offset_; client_rank < this->client_offset_ + this->n_client_; client_rank++)
        std::cout << "| " << std::setw(max_size) << client_rank << " |   client   |" << std::endl;

    for (int server_rank = this->node_offset_; server_rank < this->node_offset_ + this->n_node_; server_rank++)
        std::cout << "| " << std::setw(max_size) << server_rank << " |   server   |" << std::endl;

    std::cout << separator << std::endl << std::resetiosflags(std::ios::showbase);
}

void Controller::send_entry(int destination_rank, const std::string& command)
{
    std::cout << "Send entry to " << destination_rank << ": " << command << std::endl;

    Message message(this->next_uid_++, Message::MESSAGE_TYPE::CLIENT_CREATE_NEW_ENTRY, command);
    send_message(message, destination_rank);
}

void Controller::start_controller()
{
    std::cout << "> ";

    for (std::string line; std::getline(std::cin, line);)
    {
        std::vector<std::string> tokens;
        split_string(line, " ", tokens);

        if (!tokens.empty())
        {
            if (tokens[0] == "help")
                this->print_help();
            else if (tokens[0] == "list_ranks")
                this->list_ranks();
            else if (tokens[0] == "entry")
            {
                if (tokens.size() <= 3)
                    std::cout << "Missing arguments" << std::endl;

                int destination_rank = std::stoi(tokens[1]);

                std::vector<std::string> sub_tokens;
                auto position = split_string(line, " ", sub_tokens, 2);

                std::string command = line.substr(position, line.size() - position);

                // TODO check rank
                // TODO remove space in front of command
                this->send_entry(destination_rank, command);
            }
            else
            {
                std::cout << "Command not found: " << tokens[0] << std::endl;
                this->print_help();
            }
        }

        std::cout << "> ";
    }
}
