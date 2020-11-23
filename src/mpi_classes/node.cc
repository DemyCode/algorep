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
        std::vector<RPCQuery> queries;
        // Listen to everything
        receive_all_messages(this->rank_, 0, this->size_, queries);
        // receive_all_messages(this->rank_, this->offset_, this->n_node_, queries);

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

void Node::all_server_check(const std::vector<RPCQuery>& queries)
{
    // THIS SECTION REFER TO THE PART : RULES FOR SERVER -> ALL RULES
    if (this->commit_index_ > this->last_applied_)
    {
        last_applied_ += 1;
        // TODO : APPLY LOG LAST APPLY TO STATE MACHINE
        std::ofstream log_file("log" + std::to_string(rank_) + ".txt");
        if (log_file.is_open())
        {
            log_file << log_[last_applied_].command_ << std::endl;
            log_file.close();
        }
        else
            std::cerr << "Unable to open file : " << "log" + std::to_string(rank_) + ".txt" << std::endl;
    }

    for (const auto& query : queries)
    {
        if (query.term_ > current_term_)
        {
            current_term_ = query.term_;
            state_ = state::FOLLOWER;
        }
    }
}

void Node::follower_check(const std::vector<RPCQuery>& queries)
{
    // TODO
    for (const auto& query : queries)
    {
        if (query.type_ == RPC::RPC_TYPE::REQUEST_VOTE)
            this->handle_request_vote(query);
        if (query.type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
            this->handle_append_entries(query);
    }
    if (this->clock_.check() > election_timeout_)
        convert_to_candidate();
}

void Node::leader_check(const std::vector<RPCQuery>& queries)
{
    if (this->clock_.check() < election_timeout_ / 2)
    {
        AppendEntries empty_append = AppendEntries(this->current_term_,
                                                   this->rank_,
                                                   -1,
                                                   -1,
                                                   std::vector<Entry>(), // empty entries means heartbeat in response
                                                   this->commit_index_);
        send_to_all(this->rank_, offset_, n_node_, empty_append, 0);
        clock_.reset();
    }
    for (const auto& query : queries)
    {
        // FIXED COMMAND is probably RPC::RPC_TYPE::NEW_ENTRY
        if (query.type_ == RPC::RPC_TYPE::NEW_ENTRY)
        {
            NewEntry new_entry = std::get<NewEntry>(query.content_);
            Entry res_entry = Entry(this->current_term_, new_entry.entry_.command_);
            this->log_.push_back(res_entry);
            // TODO RESPOND AFTER APPLY TO STATE MACHINE
            // See all_server_check

            int updated_logs_count = 1;
            for (size_t i = 0; i < next_index_.size(); i++)
            {
                if ((int)i == this->rank_)
                    continue;
                if ((int)this->log_.size() - 1 >= next_index_[i])
                {
                    // check for answer from follower
                    bool success = false;
                    while (!success)
                    {
                        // send AppendEntries RPC with log entries starting at nextIndex
                        AppendEntries append_entries(this->current_term_,
                                                     this->rank_,
                                                     next_index_[i],
                                                     log_[next_index_[i]].term_,
                                                     this->log_,
                                                     this->commit_index_);
                        send_message(append_entries, i + offset_);
                        std::optional<RPCQuery> append_entries_response = wait_message(i + offset_, 50);
                        if (!append_entries_response.has_value())
                            break;
                        else if (append_entries.rpc_type_ == RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
                        {
                            auto aer = std::get<AppendEntriesResponse>(append_entries_response->content_);
                            if (aer.success_)
                            {
                                success = true;
                                next_index_[i] = next_index_[i] + 1;
                                match_index_[i] = match_index_[i] + 1;
                                updated_logs_count++;
                                if ((int)this->log_.size() - 1 > commit_index_ && updated_logs_count > n_node_ / 2
                                    && this->log_[this->log_.size() - 1].term_ == current_term_)
                                {
                                    commit_index_ = this->log_.size() - 1;
                                    NewEntryResponse new_entry_response = NewEntryResponse(new_entry.uid_, true);
                                    send_message(new_entry_response, query.source_rank_);
                                }
                            }
                            else
                                next_index_[i] = next_index_[i] - 1;
                        }
                    }
                }
            }
        }
    }
}

void Node::candidate_check(const std::vector<RPCQuery>& queries)
{
    for (const auto& query : queries)
    {
        if (query.type_ == RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
        {
            RequestVoteResponse request_vote_response = std::get<RequestVoteResponse>(query.content_);
            vote_count_ += request_vote_response.vote_granted_ ? 1 : 0;
        }
        if (query.type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
        {
            AppendEntries append_entries = std::get<AppendEntries>(query.content_);
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
    AppendEntries empty_append = AppendEntries(this->current_term_,
                                               this->rank_,
                                               69,
                                               42,
                                               std::vector<Entry>(), // empty entries means heartbeat in response
                                               this->commit_index_);
    send_to_all(offset_, n_node_, empty_append, 0);
    this->next_index_ = std::vector<int>(n_node_, this->log_.size());
    this->match_index_ = std::vector<int>(n_node_, 0);
}

void Node::handle_append_entries(const RPCQuery& query)
{
    AppendEntries append_entries = std::get<AppendEntries>(query.content_);
    if (append_entries.term_ < current_term_)
    {
        auto append_entries_response = AppendEntriesResponse(current_term_, false);
        send_message(append_entries_response, append_entries.leader_id_);
    }
    if ()
}

void Node::handle_request_vote(const RPCQuery& query)
{
    // FIXME
    auto request_vote = std::get<RequestVote>(query.content_);
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
Node::~Node()
{
    ofstream_.close();
}