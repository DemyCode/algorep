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
{}

void Node::run()
{
    bool running = true;
    std::cout << "Node " << this->rank_ << " running." << std::endl;
    while (running)
    {
        if (this->state_ == state::FOLLOWER)
            follower_run();
        if (this->state_ == state::LEADER)
            leader_run();
        if (this->state_ == state::CANDIDATE)
            candidate_run();
        // TODO Create end of loop based on REPL state::STOPPED
    }
    return;
}

void Node::follower_run()
{
    // TODO Make follower run behavior
    while (clock_.check() < election_timeout_)
    {
        for (int i = offset_; i < n_node_; i++)
        {
            // MPI CHECK ANSWER FROM VOTER
            if (i == this->rank_)
                continue;
            MPI_Status mpiStatus;
            int flag;
            MPI_Iprobe(i, 0, MPI_COMM_WORLD, &flag, &mpiStatus);
            if (!flag)
                continue;
            int buffer_size;
            MPI_Get_count(&mpiStatus, MPI_CHAR, &buffer_size);
            char* buffer = (char*)malloc(buffer_size * sizeof(char));
            MPI_Recv(buffer, buffer_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            RequestVote requestVote = RequestVote(std::string("bite"));
            AppendEntries appendEntries = AppendEntries(std::string("bite"));
            free(buffer);

            // TODO create behavior for RPC TYPE
        }
        // RESET TIMER IF RECEIVING APPENDENTRIES WITH EQUAL OR HIGHER TERM
        if (false /*something*/)
            this->clock_.reset();
    }
    // IF A CLIENT REQUEST A FOLLOWER REDIRECT IT TO LEADER

    this->state_ = state::CANDIDATE;
}

void Node::leader_run()
{
    // TODO LEADER RUN
}

void Node::candidate_run()
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
                this->handle_request_vote(response);
            }
            else if (response->type_ == RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
            {
                auto append_entries_response = std::get<AppendEntriesResponse>(response->content_);
                if (append_entries_response.term_ > this->current_term_)
                    this->state_ = state::FOLLOWER;
            };
        }
        if (count_vote > n_node_ / 2 && this->state_ == state::CANDIDATE)
            this->state_ = state::LEADER;
        if (this->state_ != state::CANDIDATE)
            return;
    }
}

void Node::handle_request_vote(const std::shared_ptr<RPCQuery>& response)
{
    auto request_vote = std::get<RequestVote>(response->content_);
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
        if (this->state_ == state::LEADER &&
            (this->voted_for_ == request_vote.candidate_id_ || this->voted_for_ == -1) &&
            (request_vote.last_log_term_ > node_last_term))
            vote_granted = true;
    }

    RequestVoteResponse request_vote_response(this->current_term_, vote_granted);
    send_message(request_vote_response, request_vote.candidate_id_);
}
