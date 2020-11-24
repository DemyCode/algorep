#pragma once

#include <mpi.h>
#include <optional>

#include "rpc-query.hh"
#include "rpc.hh"

void send_message(const RPC& rpc_message, size_t destination, int tag = 0);
std::optional<RPCQuery> send_and_wait_message(const RPC& rpc_message, size_t destination, float timeout, int tag = 0);
std::optional<RPCQuery> receive_message(size_t source, int tag = 0);
void receive_all_messages(size_t offset, size_t n_node, std::vector<RPCQuery>& queries, int tag = 0);
std::optional<RPCQuery> wait_message(size_t source, float timeout, int tag = 0);
void send_to_all(size_t offset, size_t n_node, const RPC& rpc_message, int tag = 0);