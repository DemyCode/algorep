#pragma once

#include <mpi.h>

#include "rpc-query.hh"
#include "rpc.hh"

void send_message(const RPC& rpc_message, int destination, MPI_Request& request, int tag = 0);
std::shared_ptr<RPCQuery> receive_message(int source, int tag = 0);