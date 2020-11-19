#include "node.hh"

Node::Node(int rank, int n_node, int offset, int size)
{
    this->rank_ = rank;
    this->n_node_ = n_node;
    this->offset_ = offset;
    this->size_ = size;
    this->state_ = state::FOLLOWER;

    this->currentTerm = 0;
    this->votedFor = -1;
}

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
    }
    return;
}

void Node::follower_run()
{
    using namespace std::chrono;
    auto begin = high_resolution_clock::now();
    auto end = high_resolution_clock::now();
    auto elapsed_time = duration_cast<std::chrono::milliseconds>(end - begin).count();
    while (elapsed_time < election_timeout)
    {
        // IF A CLIENT REQUEST A FOLLOWER REDIRECT IT TO LEADER

        // RESET TIMER IF RECEIVING APPENDENTRIES WITH EQUAL OR HIGHER TERM
        if (false /*something*/)
            begin = high_resolution_clock::now();

        // UPDATE TIMER
        elapsed_time = duration_cast<std::chrono::milliseconds>(end - begin).count();
    }
    this->state_ = state::CANDIDATE;
}

void Node::leader_run()
{
    // LEADER HANDLE ALL REQUEST FROM CLIENT OR SERVER REDIRECTED REQUEST
}

void Node::candidate_run()
{
    this->currentTerm += 1;
    for (int i = 0; offset_ < n_node_; i++)
    {
        RequestVote requestVote(1, 1, 1, 1);
        std::string reqser = requestVote.serialize();
        MPI_Isend(&reqser, 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_REQUEST_NULL);
    }
}
