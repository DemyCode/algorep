#include "lib-rpc.hh"

#include <iostream>

#include "message.hh"
#include "mpi_classes/process-information.hh"
#include "utils/clock.hh"

void send_message(const RPC& rpc_message, size_t destination, int tag)
{
    MPI_Request request;
    const auto& serialized_message = rpc_message.serialize();
    MPI_Isend(
        serialized_message.c_str(), serialized_message.size(), MPI_CHAR, destination, tag, MPI_COMM_WORLD, &request);
    MPI_Request_free(&request);
}

std::optional<RPCQuery> send_and_wait_message(const RPC& rpc_message, size_t destination, float timeout, int tag)
{
    send_message(rpc_message, destination, tag);
    return wait_message(destination, timeout, tag);
}

std::optional<RPCQuery> receive_message(size_t source, int tag)
{
    MPI_Status mpi_status;
    int flag;
    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, &mpi_status);

    if (!flag)
        return std::nullopt;

    int buffer_size = 0;

    MPI_Get_count(&mpi_status, MPI_CHAR, &buffer_size);

    std::vector<char> buffer(buffer_size);
    MPI_Recv(buffer.data(), buffer_size, MPI_CHAR, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::string serialized_response(buffer.begin(), buffer.end());

    auto json_response = nlohmann::json::parse(serialized_response);

    auto message_type = json_response["message_type"].get<RPC::RPC_TYPE>();
    size_t term = json_response["term"];
    auto message_content = json_response["message_content"];

    switch (message_type)
    {
        case RPC::RPC_TYPE::APPEND_ENTRIES:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(AppendEntries(term, message_content)));

        case RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(AppendEntriesResponse(term, message_content)));

        case RPC::RPC_TYPE::REQUEST_VOTE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(RequestVote(term, message_content)));

        case RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(RequestVoteResponse(term, message_content)));

        case RPC::RPC_TYPE::NEW_ENTRY:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(NewEntry(message_content)));

        case RPC::RPC_TYPE::NEW_ENTRY_RESPONSE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(NewEntryResponse(message_content)));

        case RPC::RPC_TYPE::SEARCH_LEADER:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(SearchLeader(message_content)));

        case RPC::RPC_TYPE::SEARCH_LEADER_RESPONSE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(SearchLeaderResponse(message_content)));

        case RPC::RPC_TYPE::MESSAGE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(Message(message_content)));

        case RPC::RPC_TYPE::MESSAGE_RESPONSE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(MessageResponse(message_content)));

        case RPC::RPC_TYPE::GET_STATE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(GetState(message_content)));

        case RPC::RPC_TYPE::GET_STATE_RESPONSE:
            return std::make_optional<RPCQuery>(
                source, message_type, term, RPCQuery::content_t(GetStateResponse(message_content)));
    }

    return std::nullopt;
}

void receive_all_messages(size_t offset, size_t n_node, std::vector<RPCQuery>& queries, int tag)
{
    for (size_t source_rank = offset; source_rank < offset + n_node; source_rank++)
    {
        if (source_rank == ProcessInformation::instance().rank_)
            continue;

        while (true)
        {
            auto query = receive_message(source_rank, tag);

            if (query.has_value())
                queries.emplace_back(query.value());
            else
                break;
        }
    }
}

std::optional<RPCQuery> wait_message(size_t source, float timeout, int tag)
{
    Clock clock;

    while (timeout == -1 || clock.check() < timeout)
    {
        auto query = receive_message(source, tag);
        if (query.has_value())
            return query;
    }

    return std::nullopt;
}

void send_to_all(size_t offset, size_t n_node, const RPC& rpc_message, int tag)
{
    for (size_t source_rank = offset; source_rank < offset + n_node; source_rank++)
    {
        if (ProcessInformation::instance().rank_ != source_rank)
            send_message(rpc_message, source_rank, tag);
    }
}