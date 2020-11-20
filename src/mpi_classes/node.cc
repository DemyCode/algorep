#include "node.hh"

Node::Node(int rank, int n_node, int offset, int size)
    : rank_(rank)
    , n_node_(n_node)
    , offset_(offset)
    , size_(size)
    , state_(state::FOLLOWER)
    , election_timeout(-1)
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
    Clock clock = Clock();
    // TODO Make follower run behavior
    while (clock.check() < election_timeout)
    {
        // RESET TIMER IF RECEIVING APPENDENTRIES WITH EQUAL OR HIGHER TERM
        if (false /*something*/)
            clock.reset();
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
    int count_vote = 1;
    std::vector<MPI_Request> mpi_reqs = std::vector<MPI_Request>();
    // When doing communications accross server nodes the range is [offset_ : offset_ + n_node_]
    for (int i = 0; i < n_node_; i++)
    {
        int target_rank = offset_ + i;
        mpi_reqs.push_back(MPI_Request());
        if (this->rank_ == target_rank)
            continue;
        RequestVote requestVote(this->current_term_, this->rank_, log_.size() - 1, log_[log_.size() - 1].term_);
        std::string reqser = requestVote.serialize();
        MPI_Isend(&reqser[0], reqser.size(), MPI_CHAR, target_rank, 0, MPI_COMM_WORLD, &mpi_reqs[i]);
    }

    // TODO SLEEP FOR SOME TIME
    // sleep()

    std::vector<RequestVoteResponse> vecrvr = std::vector<RequestVoteResponse>();
    std::vector<MPI_Status> vecmpis = std::vector<MPI_Status>();
    for (int i = offset_; i < n_node_; i++)
    {
        // MPI CHECK ANSWER FROM VOTER
        if (i == this->rank_)
            continue;
        MPI_Status mpiStatus;
        int flag;
        MPI_Iprobe(i, 0, MPI_COMM_WORLD, &flag, &mpiStatus);
        if (!flag)
        {
            MPI_Cancel(&mpi_reqs[i]);
            continue;
        }
        int buffer_size;
        MPI_Get_count(&mpiStatus, MPI_CHAR, &buffer_size);
        char* buffer = (char *)malloc(buffer_size * sizeof(char));
        MPI_Recv(buffer, buffer_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        RequestVoteResponse requestVoteResponse = RequestVoteResponse(std::string(buffer));
        free(buffer);

        // TODO create behavior for responses
        if (requestVoteResponse.term_ == this->current_term_)
        {

        }
        if (requestVoteResponse.vote_granted_)
        {
            count_vote += 1;
        }
    }
}
