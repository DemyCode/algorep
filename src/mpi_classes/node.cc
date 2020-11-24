#include "node.hh"

#include <ctime>

#include "mpi_classes/process-information.hh"

Node::Node()

        : state_(state_t::FOLLOWER),
          election_timeout_(),
          speed_(Message::SPEED_TYPE::HIGH),
          stop_(false),
          crash_(false),
          clock_(),
          debug_clock_(),
          vote_count_(0),
          new_entries_(),
          current_term_(0),
          voted_for_(0),
          log_(), // TODO see initialization
          commit_index_(-1),
          last_applied_(-1),
          next_index_(ProcessInformation::instance().n_node_, 0) // TODO see initialization
        , match_index_(ProcessInformation::instance().n_node_, 0) {
    this->convert_to_follower();
}

void Node::set_election_timeout()
{
    std::srand(std::time(nullptr) + ProcessInformation::instance().rank_);
    this->election_timeout_ = (float)(std::rand() % 150) + 150;
}


void Node::reset_node()
{
    this->state_ = state_t::FOLLOWER;
    this->speed_ = Message::SPEED_TYPE::HIGH;
    this->stop_ = false;
    this->crash_ = false;
    this->vote_count_ = 0;
    this->new_entries_ = std::queue<RPCQuery>();
    this->current_term_ = 0;
    this->voted_for_ = 0;
    this->log_ = std::vector<Entry>();
    this->commit_index_ = -1;
    this->last_applied_ = -1;
    this->next_index_ = std::vector<int>(ProcessInformation::instance().n_node_, log_.size());
    this->match_index_ = std::vector<int>(ProcessInformation::instance().n_node_, 0);
    this->convert_to_follower();
}

void Node::run() {
//    debug_write("Node " + std::to_string(ProcessInformation::instance().rank_) + " is running",
//                debug_clock_.check());
    while (!this->stop_) {
        if (this->crash_)
        {
            reset_node();
            this->crash_ = true;
            std::vector<RPCQuery> queries;
            receive_all_messages(0, ProcessInformation::instance().size_, queries);
            this->all_server_check(queries);
            continue;
        }
        Clock::wait(this->speed_);
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
    }
    return;
}

void Node::convert_to_candidate()
{
    // TODO IMPLEMENTATION NEEDS BE DONE
    // THIS SECTION REFERS TO : RULES FOR SERVER -> CANDIDATES -> ON CONVERSION TO CANDIDATE, START ELECTION
    // THIS FUNCTION ONLY COVERS THE FIRST POINT THE THREE OTHERS ARE HANDLED THROUGH PROBING
//    debug_write("Node " + std::to_string(ProcessInformation::instance().rank_) + " became candidate.",
//                debug_clock_.check());
    this->state_ = state_t::CANDIDATE;
    this->voted_for_ = ProcessInformation::instance().rank_;
    this->vote_count_ = 1;
    this->current_term_ += 1;
    this->clock_.reset();
    int last_log_term = -1;
    if (!log_.empty())
        last_log_term = log_.at(log_.size() - 1).term_;
    send_to_all(ProcessInformation::instance().node_offset_,
                ProcessInformation::instance().n_node_,
                RequestVote(current_term_, ProcessInformation::instance().rank_, log_.size() - 1, last_log_term));
    this->set_election_timeout();
}

void Node::all_server_check(const std::vector<RPCQuery>& queries)
{
    // THIS SECTION REFER TO THE PART : RULES FOR SERVER -> ALL RULES
    if (this->commit_index_ > this->last_applied_)
    {
        last_applied_ += 1;
        std::ofstream log_file("log" + std::to_string(ProcessInformation::instance().rank_) + ".txt");
        if (log_file.is_open())
        {
            log_file << log_.at(last_applied_).command_ << std::endl;
            log_file.close();
        }
        else
            std::cerr << "Unable to open file : "
                      << "log" + std::to_string(ProcessInformation::instance().rank_) + ".txt" << std::endl;
        if (this->state_ == state_t::LEADER)
        {
            RPCQuery entry_query = this->new_entries_.front();
            send_message(NewEntryResponse(true), entry_query.source_rank_);
            this->new_entries_.pop();
        }
    }

    for (const auto& query : queries)
    {
        if (query.term_ > current_term_)
        {
            current_term_ = query.term_;
            convert_to_follower();
        }
        if (query.type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
            handle_append_entries(query);
        if (query.type_ == RPC::RPC_TYPE::REQUEST_VOTE)
            handle_request_vote(query);

        bool success = true;
        if (query.type_ == RPC::RPC_TYPE::MESSAGE)
        {
            const auto& message = std::get<Message>(query.content_);
            switch (message.message_type_)
            {
                case Message::MESSAGE_TYPE::PROCESS_SET_SPEED:
                    this->speed_ = Message::parse_speed(message.message_content_);
                    break;
                case Message::MESSAGE_TYPE::PROCESS_STOP:
                    this->stop_ = true;
                    break;
                case Message::MESSAGE_TYPE::PROCESS_CRASH:
                    this->crash_ = true;
                    break;
                case Message::MESSAGE_TYPE::PROCESS_RECOVER:
                    this->crash_ = false;
                    break;
                default:
                    success = false;
                    break;
            }
            MessageResponse message_response(success);
            send_message(message_response, query.source_rank_);
        }
        else if (query.type_ == RPC::RPC_TYPE::GET_STATE)
        {
            GetStateResponse gs_response(this->state_);
            send_message(gs_response, query.source_rank_);
        }
        else if (query.type_ == RPC::RPC_TYPE::SEARCH_LEADER)
        {
            if (this->state_ == state_t::LEADER)
            {
                SearchLeaderResponse sl_response(ProcessInformation::instance().rank_);
                send_message(sl_response, query.source_rank_);
            }
        }
        else if (query.type_ == RPC::RPC_TYPE::NEW_ENTRY)
        {
            if (this->state_ != state_t::LEADER)
            {
                NewEntryResponse ne_response(false);
                send_message(ne_response, query.source_rank_);
            }
        }
    }
}

/*
NewEntry ->
- Renvoyer false si on est pas un LEADER
- Renvoyer true que si l'Entry a bien été copiée sur les FOLLOWER
*/

void Node::follower_check(const std::vector<RPCQuery> &queries) {
    // TODO
    (void)queries;
    if (this->clock_.check() > election_timeout_)
        convert_to_candidate();
}

std::vector<Entry> slice_vector(std::vector<Entry> vect, int begin)
{
    std::vector<Entry> res_vector = std::vector<Entry>();
    for (size_t i = begin; i < vect.size(); i++)
        res_vector.push_back(vect.at(i));
    return res_vector;
}

void Node::leader_check(const std::vector<RPCQuery>& queries)
{
    if (clock_.check() > 30.0f)
    {
//        debug_write("sending heartbeat", debug_clock_.check());
        for (size_t i = 0; i < next_index_.size(); i++)
        {
            if (i == ProcessInformation::instance().rank_)
                continue;
            if ((int)this->log_.size() - 1 >= next_index_.at(i))
            {
                // send AppendEntries RPC with log entries starting at nextIndex
                AppendEntries append_entry = AppendEntries(this->current_term_,
                                                           ProcessInformation::instance().rank_,
                                                           next_index_.at(i) - 1,
                                                           log_.at(next_index_.at(i) - 1).term_,
                                                           slice_vector(log_, next_index_.at(i)),
                                                           this->commit_index_);
                send_message(append_entry, i + ProcessInformation::instance().node_offset_);
            }
            else
            {
                AppendEntries empty_append =
                    AppendEntries(this->current_term_,
                                  ProcessInformation::instance().rank_,
                                  -1,
                                  -1,
                                  std::vector<Entry>(), // empty entries means heartbeat in response
                                  this->commit_index_);
                send_message(empty_append, i + ProcessInformation::instance().node_offset_);
            }
        }
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
            this->new_entries_.push(query);
            // TODO RESPOND AFTER APPLY TO STATE MACHINE
            // See all_server_check
        }

        if (query.type_ == RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
        {
            auto aer = std::get<AppendEntriesResponse>(query.content_);
            if (aer.success_)
            {
                next_index_.at(query.source_rank_ - ProcessInformation::instance().node_offset_) += 1;
                match_index_.at(query.source_rank_ - ProcessInformation::instance().node_offset_) += 1;
            }
            else
                next_index_.at(query.source_rank_ - ProcessInformation::instance().node_offset_) -= 1;
        }
    }

    int N = commit_index_ + 1;
    size_t majority = 1;
    for (int index_i : match_index_)
    {
        if (index_i >= N)
            majority += 1;
    }
    if (N > commit_index_ && majority >= ProcessInformation::instance().n_node_ / 2
        && log_.at(N).term_ == current_term_)
        commit_index_ = N;
}

void Node::candidate_check(const std::vector<RPCQuery>& queries)
{
    for (const auto& query : queries)
    {
        if (query.type_ == RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
        {
            RequestVoteResponse request_vote_response = std::get<RequestVoteResponse>(query.content_);
            vote_count_ += request_vote_response.vote_granted_ ? 1 : 0;
//            debug_write("Node " + std::to_string(ProcessInformation::instance().rank_) + " has "
//                            + std::to_string(vote_count_) + " vote.",
//                        debug_clock_.check());
        }
        if (query.type_ == RPC::RPC_TYPE::APPEND_ENTRIES)
        {
            AppendEntries append_entries = std::get<AppendEntries>(query.content_);
            if (append_entries.term_ >= current_term_)
                convert_to_follower();
        }
    }
    if (vote_count_ > ProcessInformation::instance().n_node_ / 2)
        convert_to_leader();
    if (clock_.check() > election_timeout_)
        convert_to_candidate();
}

void Node::convert_to_leader()
{
    debug_write("Node " + std::to_string(ProcessInformation::instance().rank_) + " became leader.",
                debug_clock_.check());
    this->state_ = state_t::LEADER;
    this->clock_.reset();
    // Envoyer heartbeat à tous les serveurs
    for (size_t i = 0; i < ProcessInformation::instance().n_node_; i++)
    {
        this->next_index_.at(i) = this->log_.size();
        this->match_index_.at(i) = -1;
    }
    AppendEntries empty_append = AppendEntries(
        this->current_term_, ProcessInformation::instance().rank_, -1, -1, std::vector<Entry>(), this->commit_index_);
    send_to_all(ProcessInformation::instance().node_offset_, ProcessInformation::instance().n_node_, empty_append, 0);
}

void Node::handle_append_entries(const RPCQuery& query)
{
//    debug_write("Node " + std::to_string(ProcessInformation::instance().rank_) + " handles AERPC",
//                debug_clock_.check());
    AppendEntries append_entries = std::get<AppendEntries>(query.content_);

    // 1 & 2
    if (append_entries.term_ < current_term_)
    {
        auto append_entries_response = AppendEntriesResponse(current_term_, false);
        send_message(append_entries_response, append_entries.leader_id_);
        return;
    }

    if (append_entries.entries_.empty())
    {
        this->clock_.reset();
        return;
    }

    if (this->log_.at(append_entries.prev_log_index_).term_ != append_entries.prev_log_term_)
    {
        auto append_entries_response = AppendEntriesResponse(append_entries.term_, false);
        send_message(append_entries_response, append_entries.leader_id_);
        return;
    }

    send_message(AppendEntriesResponse(append_entries.term_, true), append_entries.leader_id_);
    this->clock_.reset();

    std::vector<Entry> new_log = std::vector<Entry>();
    int i = 0;
    for (; i < append_entries.prev_log_index_ + 1; i++)
        new_log.push_back(log_.at(i));

    // 3 & 4.
    for (; i < (int)append_entries.entries_.size() + (append_entries.prev_log_index_ + 1); i++)
    {
        if (log_.at(i).term_ != append_entries.entries_.at(i - (append_entries.prev_log_index_ + 1)).term_)
            break;
        new_log.push_back(append_entries.entries_.at(append_entries.prev_log_index_ + i));
    }
    log_ = new_log;
    // 5.
    if (append_entries.leader_commit_ > commit_index_)
        commit_index_ = std::min(append_entries.leader_commit_, (int)append_entries.entries_.size() - 1);
}

void Node::handle_request_vote(const RPCQuery& query)
{
    RequestVote request_vote = std::get<RequestVote>(query.content_);
    if (request_vote.term_ < current_term_)
    {
        send_message(RequestVoteResponse(current_term_, false), request_vote.candidate_id_);
        return;
    }
    if (voted_for_ == -1)
    {
        if (request_vote.last_log_index_ == -1)
        {
            send_message(RequestVoteResponse(request_vote.term_, true), request_vote.candidate_id_);
            this->voted_for_ = request_vote.candidate_id_;
            return;
        }

        if (log_.at(request_vote.last_log_index_).term_ == request_vote.last_log_term_)
        {
            if (request_vote.last_log_index_ > (int)log_.size() - 1)
            {
                send_message(RequestVoteResponse(request_vote.term_, true), request_vote.candidate_id_);
                this->voted_for_ = request_vote.candidate_id_;
            }
            else
                send_message(RequestVoteResponse(request_vote.term_, false), request_vote.candidate_id_);
        }
        else if (log_.at(request_vote.last_log_index_).term_ < request_vote.last_log_term_)
        {
            this->voted_for_ = request_vote.candidate_id_;
            send_message(RequestVoteResponse(request_vote.term_, true), request_vote.candidate_id_);
        }
        else
            send_message(RequestVoteResponse(request_vote.term_, false), request_vote.candidate_id_);
    }
    else
        send_message(RequestVoteResponse(request_vote.term_, false), request_vote.candidate_id_);
}

void Node::convert_to_follower()
{
    this->state_ = state_t::FOLLOWER;
    this->voted_for_ = -1;
    this->vote_count_ = 0;
    this->clock_.reset();
    this->set_election_timeout();
//    debug_write("Node " + std::to_string(ProcessInformation::instance().rank_) + " = follower ; election_timeout "
//                    + std::to_string(this->election_timeout_),
//                debug_clock_.check());
}