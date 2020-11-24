#pragma once

#include <fstream>
#include <iostream>
#include <mpi.h>
#include <optional>
#include <string>
#include <vector>
#include <queue>

#include "rpc/append-entries.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/request-vote.hh"
#include "utils/clock.hh"
#include "utils/entry.hh"
#include "utils/debugger.hh"

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
    void run();

private:
    void set_election_timeout();
    void reset_node();

    void all_server_check(const std::vector<RPCQuery>& queries);
    void follower_check(const std::vector<RPCQuery>& queries);
    void leader_check(const std::vector<RPCQuery>& queries);
    void candidate_check(const std::vector<RPCQuery>& queries);

    void convert_to_follower();
    void convert_to_candidate();
    void convert_to_leader();

    void handle_append_entries(const RPCQuery& query);
    void handle_request_vote(const RPCQuery& query);

    state_t state_;
    float election_timeout_;
    Message::SPEED_TYPE speed_;
    bool stop_;
    bool crash_;
    Clock clock_;
    Clock debug_clock_;
    size_t vote_count_;

    std::queue<RPCQuery> new_entries_;

    // RAFT VALUES
    // Persistent on all servers (updated on stable storage before responding to RPCs)

    // latest term server has seen (initialized to 0 on first boot, increases monotonically)
    int current_term_;
    //  candidate_id that received vote in current term (or 0 if none)
    int voted_for_;
    // log entries; each entry contains command for state machine, and term when entry was received by leader (first
    // index is 1)
    std::vector<Entry> log_;

    // Volatile on all servers

    // index of highest log entry known to be committed (initialized to 0, increase monotonically)
    int commit_index_;
    // index of highest log entry applied to state machine (initialized to 0, increase monotonically)
    int last_applied_;

    // Volatile state on leader (reinitialized after election)

    // for each server, index of the next log entry to send to that server (initialized to leader last log index + 1)
    std::vector<int> next_index_;
    // for each server, index of the highest log entry known to be replicated on server (initialized to 0, increase
    // monotonically)
    std::vector<int> match_index_;
};