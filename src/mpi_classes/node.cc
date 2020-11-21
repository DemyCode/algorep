#include "node.hh"

Node::Node(int rank, int n_node, int offset, int size)
    : rank_(rank)
    , n_node_(n_node)
    , offset_(offset)
    , size_(size)
    , state_(state::FOLLOWER)
    , election_timeout_(150)
    , response_time_(20)
    , clock_()
    , current_term_(0)
    , voted_for_(-1)
    , log_() // TODO see initialization
    , commit_index_(0)
    , last_applied_(0)
    , next_index_() // TODO see initialization
    , match_index_() // TODO see initialization
    , append_entries_received_(0)
{}

void Node::run()
{
    bool running = true;
    std::cout << "Node " << this->rank_ << " running." << std::endl;
    while (running)
    {
        std::vector<std::optional<RPCQuery>> queries;
        receive_all_messages(this->offset_, this->n_node_, queries);
        this->handle_queries(queries);

        this->all_server_check(queries);

        if (this->state_ == state::FOLLOWER)
        {
            if (this->clock_.check() >= this->election_timeout_)
            {
                if (this->append_entries_received_ == 0)
                {
                    this->switch_to_candidate();
                }
                else
                {
                    this->switch_to_follower();
                }
            }
        }
        else if (this->state_ == state::CANDIDATE)
        {
            if (this->clock_.check() >= this->election_timeout_)
            {
                this->switch_to_candidate();
            }
            else
            {
                // TODO check condition here
                if (this->vote_received_ >= n_node_ / 2)
                {
                    this->switch_to_leader();
                }
            }
        }
        else if (this->state_ == state::LEADER)
        {}

        // TODO Create end of loop based on REPL state::STOPPED
        return;
    }
    return;
}

void Node::handle_queries(const std::vector<std::optional<RPCQuery>>& queries)
{
    for (const auto& query : queries)
    {
        if (query.has_value())
            continue;

        switch (query->type_)
        {
            case RPC::RPC_TYPE::APPEND_ENTRIES:
                this->handle_append_entries(query);
                break;

            case RPC::RPC_TYPE::REQUEST_VOTE:
                this->handle_request_vote(query);
                break;

            case RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE:
                this->handle_append_entries_response(query);
                break;

            case RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE:
                this->handle_request_vote_response(query);
                break;
        }
    }
}

void Node::all_server_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    // THIS SECTION REFER TO THE PART : RULES FOR SERVER -> ALL RULES
    if (this->commit_index_ > this->last_applied_)
    {
        // TODO CHECK 5.3

        last_applied_ += 1;
        // TODO : APPLY LOG LAST APPLY TO STATE MACHINE
    }

    for (const auto& query : queries)
    {
        if (query->term_ > this->current_term_)
        {
            // TODO CHECK 5.1

            this->current_term_ = query->term_;
            this->state_ = state::FOLLOWER;
        }
    }
}

void Node::follower_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    // TODO CHECK 5.2
    // TODO
}

void Node::leader_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    // TODO LEADER RUN
}

void Node::candidate_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    this->current_term_ += 1;
    this->voted_for_ = this->rank_;
    this->clock_.reset();
    int count_vote = 1;

    // When doing communications across server nodes the range is [offset_ : offset_ + n_node_]
    RequestVote request_vote_query(this->current_term_, this->rank_, log_.size() - 1, log_[log_.size() - 1].term_);
    for (int i = offset_; i < offset_ + n_node_; i++)
    {
        if (this->rank_ == i)
            continue;
        send_message(request_vote_query, i);
    }

    while (clock_.check() < election_timeout_)
    {
        for (int i = offset_; i < offset_ + n_node_; i++)
        {
            // MPI CHECK ANSWER FROM VOTER
            if (i == this->rank_)
                continue;
            auto response = receive_message(i);
            if (response == nullptr)
                continue;
            if (response->type_ == RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
            {
                auto request_vote_response = std::get<RequestVoteResponse>(response->content_);
                if (request_vote_response.vote_granted_)
                {
                    count_vote += 1;
                }
            }
            else if (response->type_ == RPC::RPC_TYPE::REQUEST_VOTE)
            {
                // TODO FUNCTION NOT DONE PLEASE REVIEW : REFER TO Request Vote RPC Receiver Implementation
                this->handle_request_vote(response);
            }
            else if (response->type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
            {
                // TODO NEED ITS OWN FUNCTION + NOT DONE PLEASE REVIEW : REFER TO AppendEntries RPC Receiver
                // Implementation
                auto append_entries = std::get<AppendEntries>(response->content_);
                if (append_entries.term_ < current_term_)
                    send_message(AppendEntriesResponse(this->current_term_, false), append_entries.leader_id_);
                else if (log_.size() < append_entries.prev_log_index_
                         || log_[append_entries.prev_log_index_].term_ != append_entries.prev_log_term_)
                    send_message(AppendEntriesResponse(this->current_term_, false), append_entries.leader_id_);
                else
                {
                    this->current_term_ = append_entries.term_;
                    this->state_ = state::FOLLOWER;
                    send_message(AppendEntriesResponse(this->current_term_, true), append_entries.leader_id_);
                }
            };
        }
        if (count_vote > n_node_ / 2 && this->state_ == state::CANDIDATE)
            this->state_ = state::LEADER;
        if (this->state_ != state::CANDIDATE)
            return;
    }
}

void Node::handle_request_vote(const std::optional<RPCQuery>& query)
{
    // FIXME
    auto request_vote = std::get<RequestVote>(query->content_);
    bool vote_granted = false;

    if (request_vote.term_ < this->current_term_)
        vote_granted = false;
    else if (request_vote.term_ > this->current_term_)
    {
        vote_granted = true;
        this->state_ = state::FOLLOWER;
        this->current_term_ = request_vote.term_;
    }
    else
    {
        int node_last_term = log_[log_.size() - 1].term_;
        if (this->state_ == state::LEADER && (this->voted_for_ == request_vote.candidate_id_ || this->voted_for_ == -1)
            && (request_vote.last_log_term_ > node_last_term))
            vote_granted = true;
    }

    RequestVoteResponse request_vote_response(this->current_term_, vote_granted);
    send_message(request_vote_response, request_vote.candidate_id_);
}

void Node::handle_append_entries(const std::optional<RPCQuery>& query)
{
    // TODO
    if (this->state_ == state::FOLLOWER)
    {
        this->append_entries_received_ += 1;
    }
    else if (this->state_ == state::CANDIDATE)
    {
        this->switch_to_follower();
    }
}

void Node::handle_request_vote_response(const std::optional<RPCQuery>& query)
{
    // TODO
    auto request_vote_response = std::get<RequestVoteResponse>(query->content_);
    if (this->state_ == state::CANDIDATE)
    {
        if (request_vote_response.vote_granted_)
            this->vote_received_ += 1;
    }
}

void Node::handle_append_entries_response(const std::optional<RPCQuery>& query)
{
    // TODO
}

void Node::switch_to_candidate()
{
    this->clock_.reset();
    this->state_ = state::CANDIDATE;
    this->vote_received_ = 0;

    this->current_term_ += 1;
    this->voted_for_ = this->rank_;

    RequestVote request_vote_query(this->current_term_, this->rank_, log_.size() - 1, log_[log_.size() - 1].term_);
    send_to_all(this->offset_, this->n_node_, request_vote_query);
}

void Node::switch_to_leader()
{
    this->clock_.reset();
    this->state_ = state::LEADER;

    // FIXME
    AppendEntries append_entries_query(this->current_term_, this->rank_, -1, -1, std::vector<Entry>(), -1);
    send_to_all(this->offset_, this->n_node_, append_entries_query);
}

void Node::switch_to_follower()
{
    this->clock_.reset();
    this->state_ = state::FOLLOWER;
    this->append_entries_received_ = 0;
}