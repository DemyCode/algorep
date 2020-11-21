#pragma once

#include <mpi.h>
#include <optional>

#include "rpc-query.hh"
#include "rpc.hh"

void send_message(const RPC& rpc_message, int destination, MPI_Request& request, int tag = 0);
void send_message(const RPC& rpc_message, int destination, int tag=0);
std::optional<RPCQuery> receive_message(int source, int tag = 0);
void receive_all_messages(int offset, int n_node, std::vector<std::optional<RPCQuery>>& queries, int tag = 0);