#include <utility>

#include "rpc-response.hh"

RPCResponse::RPCResponse(RPC::RPC_TYPE type, content_t content)
    : type_(type)
    , content_(std::move(content))
{}
