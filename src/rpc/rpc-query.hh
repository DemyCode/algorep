#pragma once

#include <variant>

#include "append-entries-response.hh"
#include "append-entries.hh"
#include "request-vote-response.hh"
#include "request-vote.hh"
#include "rpc.hh"

class RPCQuery
{
public:
    using content_t = std::variant<AppendEntries, AppendEntriesResponse, Entry, RequestVote, RequestVoteResponse>;

    RPCQuery(RPC::RPC_TYPE type, content_t content);

    RPC::RPC_TYPE type_;
    content_t content_;
};