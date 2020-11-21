#include "rpc-query.hh"

#include <utility>

RPCQuery::RPCQuery(RPC::RPC_TYPE type, int term, content_t content)
    : type_(type)
    , term_(term)
    , content_(std::move(content))
{}
