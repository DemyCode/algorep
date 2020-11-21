#pragma once

#include <iostream>
#include <mpi.h>
#include <optional>
#include <string>
#include <vector>

#include "rpc/append-entries.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/request-vote-response.hh"
#include "rpc/request-vote.hh"
#include "utils/clock.hh"
#include "utils/entry.hh"

class Node
{
public:
    enum state
    {
        FOLLOWER,
        CANDIDATE,
        LEADER,
        STOPPED
    };

    enum ping
    {
        REQUESTVOTE,
        VOTE
    };

    Node(int rank, int n_node, int offset, int size);
    void run();

private:
    void all_server_check(const std::vector<std::optional<RPCQuery>>& queries);
    void follower_check(const std::vector<std::optional<RPCQuery>>& queries);
    void leader_check(const std::vector<std::optional<RPCQuery>>& queries);
    void candidate_check(const std::vector<std::optional<RPCQuery>>& queries);

    void handle_queries(const std::vector<std::optional<RPCQuery>>& messages);
    void handle_query(const std::optional<RPCQuery>& query);
    void handle_request_vote_response(const std::optional<RPCQuery>& query);
    void handle_append_entries_response(const std::optional<RPCQuery>& query);
    void handle_request_vote(const std::optional<RPCQuery>& query);
    void handle_append_entries(const std::optional<RPCQuery>& query);

    void convert_to_candidate();
    void convert_to_leader();

    // MPI VALUES
    const int rank_;
    const int n_node_;
    const int offset_;
    const int size_;

    state state_;
    float election_timeout_;
    Clock clock_;
    int vote_count_;

    // RAFT VALUES
    // Persistent on all servers (updated on stable storage before responding to RPCs)

    // latest term server has seen (initialized to 0 on first boot, increases monotonically)
    int current_term_;
    //  candidate_id that received vote in current term (or -1 if none)
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