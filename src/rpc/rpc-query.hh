#pragma once

#include <variant>

#include "append-entries-response.hh"
#include "append-entries.hh"
#include "get-state-response.hh"
#include "get-state.hh"
#include "message-response.hh"
#include "message.hh"
#include "new-entry-response.hh"
#include "new-entry.hh"
#include "request-vote-response.hh"
#include "request-vote.hh"
#include "rpc.hh"
#include "search-leader-response.hh"
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

    RPCQuery(int source_rank, RPC::RPC_TYPE type, int term, content_t content);

    const int source_rank_;
    const RPC::RPC_TYPE type_;
    const int term_;
    const content_t content_;
};