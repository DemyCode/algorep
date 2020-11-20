#include "rpc-query.hh"

#include <utility>

RPCQuery::RPCQuery(RPC::RPC_TYPE type, content_t  content)
    : type_(type)
    , content_(std::move(content))
{}
