#pragma once

#include <mpi.h>
#include <optional>

#include "rpc-query.hh"
#include "rpc.hh"

/**
 * @brief Send a RPC message to the destination
 * 
 * @param rpc_message 
 * @param destination 
 * @param tag 
 */
void send_message(const RPC& rpc_message, size_t destination, int tag = 0);

/**
 * @brief Send a RPC message to the destination and wait for the timeout
 * 
 * @param rpc_message 
 * @param destination 
 * @param timeout 
 * @param tag 
 * @return std::optional<RPCQuery> 
 */
std::optional<RPCQuery> send_and_wait_message(const RPC& rpc_message, size_t destination, float timeout, int tag = 0);
/**
 * @brief Receive a message from the source
 * 
 * @param source 
 * @param tag 
 * @return std::optional<RPCQuery> 
 */
std::optional<RPCQuery> receive_message(size_t source, int tag = 0);
/**
 * @brief Receive messages from all nodes
 * 
 * @param offset 
 * @param n_node 
 * @param queries 
 * @param tag 
 */
void receive_all_messages(size_t offset, size_t n_node, std::vector<RPCQuery>& queries, int tag = 0);
/**
 * @brief Wait for a message from source during timeout
 * 
 * @param source 
 * @param timeout 
 * @param tag 
 * @return std::optional<RPCQuery> 
 */
std::optional<RPCQuery> wait_message(size_t source, float timeout, int tag = 0);
/**
 * @brief Send message to all nodes
 * 
 * @param offset 
 * @param n_node 
 * @param rpc_message 
 * @param tag 
 */
void send_to_all(size_t offset, size_t n_node, const RPC& rpc_message, int tag = 0);