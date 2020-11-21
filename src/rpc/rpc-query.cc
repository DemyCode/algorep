#include "rpc-query.hh"

#include <utility>

RPCQuery::RPCQuery(int source_rank, RPC::RPC_TYPE type, int term, content_t content)
    : source_rank_(source_rank)
    , type_(type)
    , term_(term)
    , content_(std::move(content))
{}
