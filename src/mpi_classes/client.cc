#include "client.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "mpi_classes/process-information.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/rpc-query.hh"

Client::Client()
    : timeout_(50) // TODO define timeout
    , speed_(Message::SPEED_TYPE::HIGH)
    , start_(false)
    , stop_(false)
    , leader_(-1)
    , leader_search_clock_()
    , entries_to_send_()
    , entry_sent_(true)
    , entry_clock_()
{}

void Client::add_command_list(const std::string& command_list_path)
{
    std::ifstream file(command_list_path);

    for (std::string line; std::getline(file, line);)
        this->entries_to_send_.emplace(Entry(-1, line));
}

void Client::run()
{
    while (!this->stop_)
    {
        Clock::wait(this->speed_);

        // If no leader, search for a new one
        if (this->start_)
        {
            if (this->leader_ == -1)
                this->search_leader();
            else if (this->entry_sent_)
                this->send_next_entry();
        }

        // Handle queries
        std::vector<RPCQuery> queries;
        receive_all_messages(0, ProcessInformation::instance().size_, queries);
        this->handle_queries(queries);

        // Check that all messages are sent
        if (this->start_)
            this->check_timeout();
    }
}

void Client::handle_queries(const std::vector<RPCQuery>& queries)
{
    for (const auto& query : queries)
    {
        if (query.type_ == RPC::RPC_TYPE::MESSAGE)
            this->handle_message(query);
        else if (query.type_ == RPC::RPC_TYPE::SEARCH_LEADER_RESPONSE)
            this->handle_search_leader_response(query);
        else if (query.type_ == RPC::RPC_TYPE::NEW_ENTRY_RESPONSE)
            this->handle_new_entry_response(query);
    }
}

void Client::handle_message(const RPCQuery& query)
{
    const auto& message = std::get<Message>(query.content_);

    bool success = true;

    switch (message.message_type_)
    {
        case Message::MESSAGE_TYPE::CLIENT_CREATE_NEW_ENTRY:
            this->entries_to_send_.emplace(Entry(-1, message.message_content_));
            break;

        case Message::MESSAGE_TYPE::CLIENT_START:
        case Message::MESSAGE_TYPE::PROCESS_RECOVER:
            this->start_ = true;
            this->reset_leader();
            break;

        case Message::MESSAGE_TYPE::PROCESS_SET_SPEED:
            this->speed_ = Message::parse_speed(message.message_content_);
            break;

        case Message::MESSAGE_TYPE::PROCESS_CRASH:
            this->start_ = false;
            this->reset_leader();
            break;

        case Message::MESSAGE_TYPE::PROCESS_STOP:
            this->stop_ = true;
            break;

        default:
            success = false;
            break;
    }

    MessageResponse message_response(message.uid_, success);
    send_message(message_response, query.source_rank_);
}

void Client::handle_search_leader_response(const RPCQuery& query)
{
    const auto& search_leader_response = std::get<SearchLeaderResponse>(query.content_);

    if (this->leader_ == -1)
        this->leader_ = search_leader_response.new_leader_;
}

void Client::handle_new_entry_response(const RPCQuery& query)
{
    const auto& new_entry_response = std::get<NewEntryResponse>(query.content_);

    if (new_entry_response.success_)
        this->entries_to_send_.pop();
    else
        this->reset_leader();

    this->entry_sent_ = true;
}

void Client::search_leader()
{
    if (this->leader_search_clock_.check() >= this->timeout_)
    {
        this->leader_search_clock_.reset();

        SearchLeader search_leader(this->leader_);
        send_to_all(ProcessInformation::instance().node_offset_, ProcessInformation::instance().n_node_, search_leader);
    }
}

void Client::send_next_entry()
{
    const auto& entry = this->entries_to_send_.front();
    send_message(entry, this->leader_);

    this->entry_sent_ = false;
    this->entry_clock_.reset();
}

void Client::check_timeout()
{
    if (this->entry_clock_.check() >= this->timeout_)
    {
        this->entry_sent_ = true;
        this->reset_leader();
    }
}

void Client::reset_leader()
{
    this->leader_ = -1;
    this->leader_search_clock_.reset();
}