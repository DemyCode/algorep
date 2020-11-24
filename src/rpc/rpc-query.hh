#pragma once

#include <variant>

#include "append-entries.hh"
#include "get-state.hh"
#include "message.hh"
#include "new-entry.hh"
#include "request-vote.hh"
#include "rpc.hh"
#include "search-leader.hh"

class RPCQuery
{
public:
    using content_t = std::variant<AppendEntries,
                                   AppendEntriesResponse,
                                   RequestVote,
                                   RequestVoteResponse,
                                   NewEntry,
                                   NewEntryResponse,
                                   SearchLeader,
                                   SearchLeaderResponse,
                                   Message,
                                   MessageResponse,
                                   GetState,
                                   GetStateResponse>;

    RPCQuery(size_t source_rank, RPC::RPC_TYPE type, int term, content_t content);

    const size_t source_rank_;
    const RPC::RPC_TYPE type_;
    const int term_;
    const content_t content_;
};