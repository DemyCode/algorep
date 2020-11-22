#include "client.hh"

#include <iostream>
#include <vector>

#include "mpi_classes/process-information.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/rpc-query.hh"

Client::Client()
    : timeout_(50)
    , speed_(Message::SPEED_TYPE::HIGH)
    , start_(false)
    , stop_(false)
    , leader_(-1)
    , leader_search_clock_()
    , next_uid_(0)
    , entries_to_send_()
    , entries_()
    , entries_clocks_()
    , entries_leaders_()
{}

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
            else
                this->send_entries();
        }

        // Handle queries
        std::vector<RPCQuery> queries;
        receive_all_messages(ProcessInformation::instance().rank_, 0, ProcessInformation::instance().size_, queries);
        this->handle_queries(queries);

        // Check that all messages are sent
        if (this->start_)
        {
            this->check_timeouts();
        }
    }
}

void Client::handle_queries(const std::vector<RPCQuery>& queries)
{
    for (const auto& query : queries)
    {
        if (query.type_ == RPC::RPC_TYPE::MESSAGE)
        {
            this->handle_message(query);
        }
        else if (query.type_ == RPC::RPC_TYPE::SEARCH_LEADER_RESPONSE)
        {
            this->handle_search_leader_response(query);
        }
        else if (query.type_ == RPC::RPC_TYPE::NEW_ENTRY_RESPONSE)
        {
            this->handle_new_entry_response(query);
        }
    }
}

void Client::handle_message(const RPCQuery& query)
{
    const auto& message = std::get<Message>(query.content_);

    bool success = true;

    switch (message.message_type_)
    {
        case Message::MESSAGE_TYPE::CLIENT_CREATE_NEW_ENTRY:
            this->entries_to_send_.emplace(this->next_uid_++, Entry(-1, message.message_content_));
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

    const auto& entry = this->entries_.find(new_entry_response.uid_);

    if (!new_entry_response.success_)
    {
        this->entries_to_send_.emplace(entry->second);
        this->reset_leader();
    }

    this->entries_.erase(new_entry_response.uid_);
    this->entries_clocks_.erase(new_entry_response.uid_);
    this->entries_leaders_.erase(new_entry_response.uid_);
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

void Client::send_entries()
{
    while (!this->entries_to_send_.empty())
    {
        const auto& entry = this->entries_to_send_.front();

        send_message(entry, this->leader_);

        this->entries_to_send_.pop();
        this->entries_.emplace(entry.uid_, entry);
        this->entries_clocks_.emplace(entry.uid_, Clock());
        this->entries_leaders_.emplace(entry.uid_, this->leader_);
    }
}

void Client::check_timeouts()
{
    for (const auto& element : this->entries_clocks_)
    {
        if (element.second.check() >= this->timeout_)
        {
            auto uid = element.first;
            const auto& entry = this->entries_.find(uid);
            const auto& leader = this->entries_leaders_.find(uid);

            this->entries_to_send_.emplace(entry->second);

            if (this->leader_ == leader->second)
                this->reset_leader();

            this->entries_.erase(uid);
            this->entries_clocks_.erase(uid);
            this->entries_leaders_.erase(uid);
        }
    }
}

void Client::reset_leader()
{
    this->leader_ = -1;
    this->leader_search_clock_.reset();
}