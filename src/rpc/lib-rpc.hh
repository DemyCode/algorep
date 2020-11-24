#pragma once

#include <mpi.h>
#include <optional>

#include "rpc-query.hh"
#include "rpc.hh"

void send_message(const RPC& rpc_message, int destination, MPI_Request& request, int tag = 0);
void send_message(const RPC& rpc_message, int destination, int tag = 0);
std::optional<RPCQuery> send_and_wait_message(const RPC& rpc_message, int destination, float timeout, int tag = 0);
std::optional<RPCQuery> receive_message(int source, int tag = 0);
void receive_all_messages(int offset, int n_node, std::vector<RPCQuery>& queries, int tag = 0);
std::optional<RPCQuery> wait_message(int source, float timeout, int tag = 0);
void send_to_all(int offset, int n_node, const RPC& rpc_message, int tag = 0);