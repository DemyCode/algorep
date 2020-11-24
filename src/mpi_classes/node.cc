#include "node.hh"

#include <ctime>

#include "mpi_classes/process-information.hh"

Node::Node()
    : state_(state_t::FOLLOWER)
    , election_timeout_(0)
    , clock_()
    , vote_count_(0)
    , current_term_(0)
    , voted_for_(0)
    , log_() // TODO see initialization
    , commit_index_(0)
    , last_applied_(0)
    , next_index_(ProcessInformation::instance().n_node_) // TODO see initialization
    , match_index_(ProcessInformation::instance().n_node_, 0)
{
    this->set_election_timeout();
}

void Node::set_election_timeout()
{
    std::srand(std::time(nullptr) + ProcessInformation::instance().rank_);
    this->election_timeout_ = (float)(std::rand() % 150) + 150;
}

void Node::run()
{
    bool running = true;
    std::cout << "Node " << ProcessInformation::instance().rank_ << " running." << std::endl;
    while (running)
    {
        std::vector<RPCQuery> queries;
        // Listen to everything
        receive_all_messages(0, ProcessInformation::instance().size_, queries);
        // receive_all_messages(this->rank_, this->offset_, this->n_node_, queries);

        this->all_server_check(queries);

        if (this->state_ == state_t::FOLLOWER)
            this->follower_check(queries);

        else if (this->state_ == state_t::CANDIDATE)
            this->candidate_check(queries);

        else if (this->state_ == state_t::LEADER)
            this->leader_check(queries);

        // TODO Create end of loop based on REPL state::STOPPED
        // FIXME remove return
        return;
    }
    return;
}

void Node::convert_to_candidate()
{
    // TODO IMPLEMENTATION NEEDS BE DONE
    // THIS SECTION REFERS TO : RULES FOR SERVER -> CANDIDATES -> ON CONVERSION TO CANDIDATE, START ELECTION
    // THIS FUNCTION ONLY COVERS THE FIRST POINT THE THREE OTHERS ARE HANDLED THROUGH PROBING
    this->state_ = state_t::CANDIDATE;
    this->voted_for_ = ProcessInformation::instance().rank_;
    this->vote_count_ = 1;
    this->clock_.reset();
    this->set_election_timeout();
    // When doing communications across server nodes the range is [offset_ : offset_ + n_node_]
    RequestVote request_vote_query(
        this->current_term_, ProcessInformation::instance().rank_, log_.size() - 1, log_[log_.size() - 1].term_);
    for (size_t i = ProcessInformation::instance().node_offset_;
         i < ProcessInformation::instance().node_offset_ + ProcessInformation::instance().n_node_;
         i++)
    {
        if (ProcessInformation::instance().rank_ == i)
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
        std::ofstream log_file("log" + std::to_string(ProcessInformation::instance().rank_) + ".txt");
        if (log_file.is_open())
        {
            log_file << log_[last_applied_].command_ << std::endl;
            log_file.close();
        }
        else
            std::cerr << "Unable to open file : "
                      << "log" + std::to_string(ProcessInformation::instance().rank_) + ".txt" << std::endl;
    }

    for (const auto& query : queries)
    {
        if (query.term_ > current_term_)
        {
            current_term_ = query.term_;
            state_ = state_t::FOLLOWER;
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

std::vector<Entry> slice_vector(std::vector<Entry> vect, int begin)
{
    std::vector<Entry>::const_iterator first = vect.begin() + begin;
    std::vector<Entry>::const_iterator last = vect.end();
    return std::vector<Entry>(first, last);
}

void Node::leader_check(const std::vector<RPCQuery>& queries)
{
    if (this->clock_.check() < election_timeout_ / 2)
    {
        AppendEntries empty_append = AppendEntries(this->current_term_,
                                                   ProcessInformation::instance().rank_,
                                                   0,
                                                   0,
                                                   std::vector<Entry>(), // empty entries means heartbeat in response
                                                   this->commit_index_);
        send_to_all(
            ProcessInformation::instance().node_offset_, ProcessInformation::instance().n_node_, empty_append, 0);
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

            size_t updated_logs_count = 1;
            for (size_t i = 0; i < next_index_.size(); i++)
            {
                if (i == ProcessInformation::instance().rank_)
                    continue;
                if (this->log_.size() - 1 >= next_index_[i])
                {
                    // check for answer from follower
                    bool success = false;
                    while (!success)
                    {
                        // send AppendEntries RPC with log entries starting at nextIndex
                        AppendEntries append_entries(this->current_term_,
                                                     ProcessInformation::instance().rank_,
                                                     next_index_[i] - 1,
                                                     log_[next_index_[i] - 1].term_,
                                                     slice_vector(log_, next_index_[i]),
                                                     this->commit_index_);
                        send_message(append_entries, i + ProcessInformation::instance().node_offset_);
                        std::optional<RPCQuery> append_entries_response =
                            wait_message(i + ProcessInformation::instance().node_offset_, 50);
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
                                if (this->log_.size() - 1 > commit_index_
                                    && updated_logs_count > ProcessInformation::instance().n_node_ / 2
                                    && this->log_[this->log_.size() - 1].term_ == current_term_)
                                {
                                    commit_index_ = this->log_.size() - 1;
                                    NewEntryResponse new_entry_response = NewEntryResponse(true);
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
                this->state_ = state_t::FOLLOWER;
        }
    }
    if (vote_count_ > ProcessInformation::instance().n_node_ / 2)
        convert_to_leader();
    if (clock_.check() > election_timeout_)
        convert_to_candidate();
}

void Node::convert_to_leader()
{
    this->state_ = state_t::LEADER;
    this->clock_.reset();
    // Envoyer heartbeat à tous les serveurs
    AppendEntries empty_append = AppendEntries(this->current_term_,
                                               ProcessInformation::instance().rank_,
                                               69,
                                               42,
                                               std::vector<Entry>(), // empty entries means heartbeat in response
                                               this->commit_index_);
    send_to_all(ProcessInformation::instance().node_offset_, ProcessInformation::instance().n_node_, empty_append, 0);
    this->next_index_ = std::vector<size_t>(ProcessInformation::instance().n_node_, this->log_.size());
    this->match_index_ = std::vector<size_t>(ProcessInformation::instance().n_node_, 0);
}

void Node::handle_append_entries(const RPCQuery& query)
{
    AppendEntries append_entries = std::get<AppendEntries>(query.content_);
    // 1 & 2
    if (append_entries.term_ < current_term_)
    {
        auto append_entries_response = AppendEntriesResponse(current_term_, false);
        send_message(append_entries_response, append_entries.leader_id_);
    }

    if (this->log_[append_entries.prev_log_index_].term_ != append_entries.prev_log_term_)
    {
        auto append_entries_response = AppendEntriesResponse(append_entries.term_, false);
        send_message(append_entries_response, append_entries.leader_id_);
    }

    std::vector<Entry> new_log = std::vector<Entry>();
    size_t i = 0;
    for (; i < append_entries.prev_log_index_; i++)
        new_log.push_back(log_[i]);

    // 3 & 4.
    for (; i < append_entries.entries_.size() + append_entries.prev_log_index_; i++)
        new_log.push_back(append_entries.entries_[append_entries.prev_log_index_ + i]);
    log_ = new_log;
    // 5.
    if (append_entries.leader_commit_ > commit_index_)
        commit_index_ = std::min(append_entries.leader_commit_, append_entries.entries_.size() - 1);
}

void Node::handle_request_vote(const RPCQuery& query)
{
    // FIXME VOTEDFOR ON S'EN BAT LES COUILLES (POUR L'INSTANT)
    RequestVote request_vote = std::get<RequestVote>(query.content_);
    if (request_vote.term_ < current_term_)
    {
        send_message(RequestVoteResponse(current_term_, false), request_vote.candidate_id_);
        return;
    }
    if (log_[request_vote.last_log_index_].term_ == request_vote.last_log_term_)
    {
        if (request_vote.last_log_index_ > log_.size() - 1)
            send_message(RequestVoteResponse(request_vote.term_, true), request_vote.candidate_id_);
        else
            send_message(RequestVoteResponse(request_vote.term_, false), request_vote.candidate_id_);
    }
    else if (log_[request_vote.last_log_index_].term_ < request_vote.last_log_term_)
    {
        send_message(RequestVoteResponse(request_vote.term_, true), request_vote.candidate_id_);
    }
    else
        send_message(RequestVoteResponse(request_vote.term_, false), request_vote.candidate_id_);
}

Node::~Node()
{
    ofstream_.close();
}