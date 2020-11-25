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

    /**
     * @brief Constructor for Querying the messages
     * @param source_rank rank of query
     * @param term term of query
     * @param content Content type for appropriating casting
     */
    RPCQuery(size_t source_rank, RPC::RPC_TYPE type, int term, content_t content);

    // id of sender
    const size_t source_rank_;
    // type of the RPC
    const RPC::RPC_TYPE type_;
    // term of the RPC
    const int term_;
    // content of the RPC; the RPC itself
    const content_t content_;
};