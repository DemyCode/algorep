#include "client.hh"

#include <vector>

#include "rpc/lib-rpc.hh"
#include "rpc/rpc-query.hh"

Client::Client(/*int rank, int n_client, int client_offset,*/ int n_node, int node_offset, int size)
    /*: rank_(rank)
    , n_client_(n_client)
    , client_offset_(client_offset)*/
    : n_node_(n_node)
    , node_offset_(node_offset)
    , size_(size)
    , timeout_(50)
    , leader_(-1)
    , next_uid_(0)
    , entries_to_send_()
    , entries_()
    , entries_clocks_()
    , entries_leaders_()
{}

void Client::run()
{
    while (true)
    {
        // If no leader, search for a new one
        if (this->leader_ == -1)
            this->search_leader();
        else
            this->send_entries();

        // Handle queries
        std::vector<RPCQuery> queries;
        receive_all_messages(0, this->size_, queries);
        this->handle_queries(queries);

        // Check that all messages are sent
        this->check_timeouts();

        // FIXME remove return
        return;
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

    if (message.message_type_ == Message::MESSAGE_TYPE::CLIENT_CREATE_NEW_ENTRY)
    {
        int entry_uid = this->next_uid_++;
        this->entries_to_send_.emplace(entry_uid, Entry(-1, message.message_content_));

        MessageResponse message_response(message.uid_, true);
        send_message(message_response, query.source_rank_);
    }
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
        this->entries_to_send_.emplace(entry->second);

    this->entries_.erase(new_entry_response.uid_);
    this->entries_clocks_.erase(new_entry_response.uid_);
    this->entries_leaders_.erase(new_entry_response.uid_);
}

void Client::search_leader() const
{
    SearchLeader search_leader(this->leader_);
    send_to_all(this->node_offset_, this->n_node_, search_leader);
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
                this->leader_ = -1;

            this->entries_.erase(uid);
            this->entries_clocks_.erase(uid);
            this->entries_leaders_.erase(uid);
        }
    }
}