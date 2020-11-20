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

    // When doing communications accross server nodes the range is [offset_ : offset_ + n_node_]
    RequestVote request_vote(this->current_term_, this->rank_, log_.size() - 1, log_[log_.size() - 1].term_);
    for (int i = offset_; i < offset_ + n_node_; i++)
    {
        if (this->rank_ == i)
            continue;
        MPI_Request mpi_request;
        send_message(request_vote, i, mpi_request);
        MPI_Request_free(&mpi_request);
    }

    while (clock_.check() < election_timeout_)
    {
        for (int i = offset_; i < offset_ + n_node_; i++)
        {
            // MPI CHECK ANSWER FROM VOTER
            if (i == this->rank_)
                continue;
            MPI_Status mpi_status;
            int flag;
            MPI_Iprobe(i, 0, MPI_COMM_WORLD, &flag, &mpi_status);
            if (!flag)
                continue;
            RPCResponse response = receive_message(i);
            if (response == nullptr)
            {

            }
            else if (response.type_ == RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
            {

            }
            else if (response.type_ == RPC::RPC_TYPE::REQUEST_VOTE)
            {

            }
            if (response.type_ == RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
            {

            }
        }
    }
}
