#pragma once

#include <fstream>
#include <iostream>
#include <mpi.h>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "rpc/append-entries.hh"
#include "rpc/lib-rpc.hh"
#include "rpc/request-vote.hh"
#include "utils/clock.hh"
#include "utils/debugger.hh"
#include "utils/entry.hh"

class Node
{
public:
    using state_t = GetStateResponse::STATE;

    /// Constructor
    Node();

    /**
     ** @brief Main function for the node that check the state of a node in order to decide what to do with the messages
     ** received
     */
    void run();

private:
    /**
     ** @brief Set the election timeout for the node between 150ms and 300ms
     */
    void set_election_timeout();

    /**
     ** @brief Used to reset the node attributes when the node crash
     */
    void reset_node();

    /**
     ** @brief Check the type of incoming query regardless the type of the node and decide what to do with this node
     ** @param queries list of queries to handle
     */
    void all_server_check(const std::vector<RPCQuery>& queries);

    /**
     * @brief If the follower didn't receive any heartbeat for at least more than the election timeout call the
     ** convert_to_candidate function
     */
    void follower_check();

    /**
     ** @brief This function call the log replication and see if it worked on a majority of the followers
     ** @param queries list of queries to handle
     */
    void leader_check(const std::vector<RPCQuery>& queries);

    /**
     ** @brief This function start an election, vote for himself send a RequestVote to all other node and check the
     ** result of the election
     ** @param queries list of queries to handle
     */
    void candidate_check(const std::vector<RPCQuery>& queries);

    /**
     ** @brief  Convert a node to a follower
     */
    void convert_to_follower();

    /**
     * @brief Convert a node to a candidate
     *
     */
    void convert_to_candidate();

    /**
     ** @brief Convert a node to a Leader and send a heartbeat to the other nodes
     */
    void convert_to_leader();

    /**
     ** @brief This function handle the log replication
     ** @param query query to handle
     */
    void handle_append_entries(const RPCQuery& query);

    /**
     ** @brief This function handle the vote for an election in order to decide a new Leader
     ** @param query query to handle
     */
    void handle_request_vote(const RPCQuery& query);

    /**
     ** @brief This function handle the incoming messages and decide what to do depending on them
     ** @param query query to handle
     */
    void handle_message(const RPCQuery& query);

    /// state of the node
    state_t state_;
    /// election timeout of the node
    float election_timeout_;
    /// heartbeat timeout of the node
    float heartbeat_timeout_;
    /// current speed of the process
    Message::SPEED_TYPE speed_;
    /// true if the process is stopped
    bool stop_;
    /// process clock
    Clock clock_;
    /// current vote count
    size_t vote_count_;

    /// queue of new entries to handle
    std::queue<RPCQuery> new_entries_;

    // RAFT VALUES
    // Persistent on all servers (updated on stable storage before responding to RPCs)

    /// latest term server has seen (initialized to 0 on first boot, increases monotonically)
    int current_term_;
    ///  candidate_id that received vote in current term (or 0 if none)
    size_t voted_for_;
    /// log entries; each entry contains command for state machine, and term when entry was received by leader (first
    /// index is 1)
    std::vector<Entry> log_;

    // Volatile on all servers

    /// index of highest log entry known to be committed (initialized to 0, increase monotonically)
    int commit_index_;
    /// index of highest log entry applied to state machine (initialized to 0, increase monotonically)
    int last_applied_;

    // Volatile state on leader (reinitialized after election)

    /// for each server, index of the next log entry to send to that server (initialized to leader last log index + 1)
    std::vector<int> next_index_;
    /// for each server, index of the highest log entry known to be replicated on server (initialized to 0, increase
    /// monotonically)
    std::vector<int> match_index_;
};