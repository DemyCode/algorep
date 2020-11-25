#pragma once

#include <map>
#include <queue>

#include "rpc/rpc-query.hh"
#include "utils/clock.hh"

class Client
{
public:
    /** @brief Constructor
     */
    Client();

    /** @brief run the client
     */
    void run();
    /** @brief add a list of command to the queue
     ** @param command_list_path path of the file containing the commands
     */
    void add_command_list(const std::string& command_list_path);

private:
    /** @brief handle a list of queries
     ** @param queries list of queries
     */
    void handle_queries(const std::vector<RPCQuery>& queries);

    /** @brief handle a message
     ** @param query the query
     */
    void handle_message(const RPCQuery& query);

    /** @brief handle a search leader response
     ** @param query the query
     */
    void handle_search_leader_response(const RPCQuery& query);

    /** @brief handle a new entry response
     ** @param query the query
     */
    void handle_new_entry_response(const RPCQuery& query);

    /** @brief search for a leader
     */
    void search_leader();

    /** @brief send the next entry in the queue
     */
    void send_next_entry();

    /** @brief check if the timeout is passed
     */
    void check_timeout();

    /** @brief reset the current leader
     */
    void reset_leader();

    /** @brief Send a wait finish to the controller
     */
    void send_wait_finish();

    /// timeout of the client
    const float timeout_;

    /// current speed of the process
    Message::SPEED_TYPE speed_;

    /// true if the client has started
    bool start_;
    /// true if the client his stopped
    bool stop_;

    /// current leader, 0 if none
    size_t leader_;
    /// search leader clock (avoid spamming with RPC)
    Clock leader_search_clock_;

    /// queue of entries to send
    std::queue<NewEntry> entries_to_send_;

    /// false when the next entry in the queue is not committed on the leader
    bool entry_sent_;
    /// clock for the entry (check if the leader is dead)
    Clock entry_clock_;

    /// queue of rank to send the wait finish to
    std::queue<size_t> wait_finish_;
};
