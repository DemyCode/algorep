#include "controller.hh"

#include <iostream>
#include <string>
#include <vector>

#include "rpc/lib-rpc.hh"
#include "rpc/message.hh"
#include "utils/string-utils.hh"

static void print_help()
{
    std::cout << "Available commands:" << std::endl;
    std::cout << "help: print this message" << std::endl;
    std::cout << "entry [client_id] [command]: send an entry to the client_id" << std::endl;
    // TODO add list ranks command
}

static void send_entry(int destination_rank, const std::string& command)
{
    std::cout << "Send entry to " << destination_rank << ": " << command << std::endl;
    // FIXME uid
    Message message(0, Message::MESSAGE_TYPE::CLIENT_CREATE_NEW_ENTRY, command);
    send_message(message, destination_rank);
}

void start_controller()
{
    std::cout << "> ";

    for (std::string line; std::getline(std::cin, line);)
    {
        std::vector<std::string> tokens;
        split_string(line, " ", tokens);

        if (!tokens.empty())
        {
            if (tokens[0] == "entry")
            {
                int destination_rank = std::stoi(tokens[1]);

                std::vector<std::string> sub_tokens;
                auto position = split_string(line, " ", sub_tokens, 2);

                std::string command = line.substr(position, line.size() - position);

                send_entry(destination_rank, command);
            }
            else
            {
                print_help();
            }
        }

        std::cout << "> ";
    }
}
