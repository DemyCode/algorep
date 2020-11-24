#pragma once

#include <fstream>
#include <iostream>
#include <mpi.h>
#include <optional>
#include <string>
#include <vector>

#include "rpc/append-entries.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/request-vote.hh"
#include "utils/clock.hh"
#include "utils/entry.hh"

class Node
{
public:
    using state_t = GetStateResponse::STATE;

    enum ping
    {
        REQUESTVOTE,
        VOTE
    };

    Node();
    ~Node();
    void run();

private:
    void set_election_timeout();
    void convert_to_candidate();

    void all_server_check(const std::vector<RPCQuery>& queries);
    void follower_check(const std::vector<RPCQuery>& queries);
    void leader_check(const std::vector<RPCQuery>& queries);
    void candidate_check(const std::vector<RPCQuery>& queries);

    void convert_to_leader();

    void handle_append_entries(const RPCQuery& query);
    void handle_request_vote(const RPCQuery& query);

    state_t state_;
    float election_timeout_;
    Clock clock_;
    size_t vote_count_;

    std::ofstream ofstream_;

    // RAFT VALUES
    // Persistent on all servers (updated on stable storage before responding to RPCs)

    // latest term server has seen (initialized to 0 on first boot, increases monotonically)
    int current_term_;
    //  candidate_id that received vote in current term (or 0 if none)
    size_t voted_for_;
    // log entries; each entry contains command for state machine, and term when entry was received by leader (first
    // index is 1)
    std::vector<Entry> log_;

    // Volatile on all servers

    // index of highest log entry known to be committed (initialized to 0, increase monotonically)
    size_t commit_index_;
    // index of highest log entry applied to state machine (initialized to 0, increase monotonically)
    size_t last_applied_;

    // Volatile state on leader (reinitialized after election)

    // for each server, index of the next log entry to send to that server (initialized to leader last log index + 1)
    std::vector<size_t> next_index_;
    // for each server, index of the highest log entry known to be replicated on server (initialized to 0, increase
    // monotonically)
    std::vector<size_t> match_index_;
};