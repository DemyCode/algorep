#include "node.hh"

Node::Node(int rank, int n_node, int offset, int size)
    : rank_(rank)
    , n_node_(n_node)
    , offset_(offset)
    , size_(size)
    , state_(state::FOLLOWER)
    , election_timeout_(150)
    , clock_()
    , vote_count_(0)
    , current_term_(0)
    , voted_for_(-1)
    , log_() // TODO see initialization
    , commit_index_(0)
    , last_applied_(0)
    , next_index_() // TODO see initialization
    , match_index_() // TODO see initialization
{}

void Node::run()
{
    bool running = true;
    std::cout << "Node " << this->rank_ << " running." << std::endl;
    while (running)
    {
        std::vector<std::optional<RPCQuery>> queries;
        receive_all_messages(this->offset_, this->n_node_, queries);

        this->all_server_check(queries);

        if (this->state_ == state::FOLLOWER)
            this->follower_check(queries);

        else if (this->state_ == state::CANDIDATE)
            this->candidate_check(queries);

        else if (this->state_ == state::LEADER)
            this->leader_check(queries);

        // TODO Create end of loop based on REPL state::STOPPED
        return;
    }
    return;
}

void Node::convert_to_candidate()
{
    // TODO IMPLEMENTATION NEEDS BE DONE
    // THIS SECTION REFERS TO : RULES FOR SERVER -> CANDIDATES -> ON CONVERSION TO CANDIDATE, START ELECTION
    // THIS FUNCTION ONLY COVERS THE FIRST POINT THE THREE OTHERS ARE HANDLED THROUGH PROBING
    this->state_ = state::CANDIDATE;
    this->voted_for_ = this->rank_;
    this->vote_count_ = 1;
    this->clock_.reset();
    // When doing communications across server nodes the range is [offset_ : offset_ + n_node_]
    RequestVote request_vote_query(this->current_term_, this->rank_, log_.size() - 1, log_[log_.size() - 1].term_);
    for (int i = offset_; i < offset_ + n_node_; i++)
    {
        if (this->rank_ == i)
            continue;
        send_message(request_vote_query, i);
    }
}

void Node::all_server_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    // THIS SECTION REFER TO THE PART : RULES FOR SERVER -> ALL RULES
    if (this->commit_index_ > this->last_applied_)
    {
        last_applied_ += 1;
        // TODO : APPLY LOG LAST APPLY TO STATE MACHINE
    }

    for (const auto& query : queries)
    {
        if (query == std::nullopt)
            continue;
        if (query->term_ > current_term_)
        {
            current_term_ = query->term_;
            state_ = state::FOLLOWER;
        }
    }
}

void Node::follower_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    // TODO
    for (const auto& query : queries)
    {
        if (!query.has_value())
            continue;
        if (query->type_ == RPC::RPC_TYPE::REQUEST_VOTE)
            this->handle_request_vote(query);
        if (query->type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
            this->handle_append_entries(query);
    }
    if (this->clock_.check() > election_timeout_)
        convert_to_candidate();
}

void Node::leader_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    for (const auto& query : queries)
    {
        if (!query.has_value())
            continue;
        // this->log_.append(query->content_);

        if (query->type_ == COMMAND)
        {
            Entry new_entry = Entry(this->current_term_, COMMAND);
            this->log_.push_back(new_entry);
            for (int i = offset_; i < offset_ + n_node_; i++)
            {
                if (this->rank_ == i)
                    continue;
                send_message(AppendEntries(this->current_term_,
                                           this->rank_,
                                           this->log_.size() - 2,
                                           this->log_[this->log_.size() - 2].term_,
                                           std::vector<Entry>{new_entry},
                                           this->commit_index_));
            }
        }
    }
}

void Node::candidate_check(const std::vector<std::optional<RPCQuery>>& queries)
{
    for (const auto& query : queries)
    {
        if (!query.has_value())
            continue;
        if (query->type_ == RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
        {
            RequestVoteResponse request_vote_response = std::get<RequestVoteResponse>(query->content_);
            vote_count_ += request_vote_response.vote_granted_ ? 1 : 0;
        }
        if (query->type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
        {
            AppendEntries append_entries = std::get<AppendEntries>(query->content_);
            if (append_entries.term_ > current_term_)
                this->state_ = state::FOLLOWER;
        }
    }
    if (vote_count_ > n_node_ / 2)
        convert_to_leader();
    if (clock_.check() > election_timeout_)
        convert_to_candidate();
}

void Node::convert_to_leader()
{
    this->state_ = state::LEADER;
    this->clock_.reset();
    // Envoyer heartbeat à tous les serveurs
    AppendEntries empty_append = AppendEntries(this->current_term_, this->rank_,
                                               // int prev_log_index,
                                               0,
                                               // int prev_log_term,
                                               0,
                                               std::vector<Entry>(),
                                               this->commit_index_);
    send_to_all(offset_, n_node_, empty_append, 0);
}

void Node::handle_append_entries(const std::optional<RPCQuery>& query)
{
    AppendEntries append_entry = std::get<AppendEntries>(query->content_);

    if (append_entry.term_ < this->current_term_)
    { // 1.
        send_message(AppendEntriesResponse(this->current_term_, false), append_entry.leader_id_);
        return;
    }
    if ((int)log_.size() > append_entry.prev_log_index_
        && this->log_[append_entry.prev_log_index_].term_ != append_entry.prev_log_term_)
    { // 2.
        send_message(AppendEntriesResponse(append_entry.term_, false), append_entry.leader_id_);
        return;
    }
    send_message(AppendEntriesResponse(append_entry.term_, true), append_entry.leader_id_);
    std::vector<Entry> new_log = std::vector<Entry>();
    for (int i = 0; i < append_entry.prev_log_index_; i++)
        new_log.push_back(log_[i]);
    size_t i = 0;
    for (; i < append_entry.entries_.size(); i++)
    {
        if (log_[append_entry.prev_log_index_ + i].term_ == append_entry.entries_[i].term_)
            new_log.push_back(log_[append_entry.prev_log_index_ + i]);
        else
            break;
    }
    i -= 1;
    for (; i < append_entry.entries_.size(); i++)
        new_log.push_back(append_entry.entries_[append_entry.prev_log_index_ + i]);
    log_ = new_log;
    if (append_entry.leader_commit_ > commit_index_)
        commit_index_ = std::min(append_entry.leader_commit_, (int)append_entry.entries_.size() - 1);
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