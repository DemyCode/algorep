#include "lib-rpc.hh"

void send_message(const RPC& rpc_message, int destination, MPI_Request& request, int tag)
{
    const auto& serialized_message = rpc_message.serialize();
    MPI_Isend(
        serialized_message.c_str(), serialized_message.size(), MPI_CHAR, destination, tag, MPI_COMM_WORLD, &request);
}

void send_message(const RPC& rpc_message, int destination, int tag)
{
    MPI_Request request;
    send_message(rpc_message, destination, request, tag);
    MPI_Request_free(&request);
}

std::shared_ptr<RPCQuery> receive_message(int source, int tag)
{
    MPI_Status mpi_status;
    int flag;
    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, &mpi_status);

    if (!flag)
        return nullptr;

    int buffer_size = 0;

    MPI_Get_count(&mpi_status, MPI_CHAR, &buffer_size);

    std::vector<char> buffer(buffer_size);
    MPI_Recv(buffer.data(), buffer_size, MPI_CHAR, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::string serialized_response(buffer.begin(), buffer.end());

    auto json_response = nlohmann::json::parse(serialized_response);

    auto message_type = json_response["message_type"].get<RPC::RPC_TYPE>();
    auto message_content = json_response["message_content"];

    switch (message_type)
    {
        case RPC::RPC_TYPE::APPEND_ENTRIES:
            return std::make_shared<RPCQuery>(
                RPCQuery(message_type, RPCQuery::content_t(AppendEntries(message_content))));

        case RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE:
            return std::make_shared<RPCQuery>(
                RPCQuery(message_type, RPCQuery::content_t(AppendEntriesResponse(message_content))));

        case RPC::RPC_TYPE::REQUEST_VOTE:
            return std::make_shared<RPCQuery>(
                RPCQuery(message_type, RPCQuery::content_t(RequestVote(message_content))));

        case RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE:
            return std::make_shared<RPCQuery>(
                RPCQuery(message_type, RPCQuery::content_t(RequestVoteResponse(message_content))));
    }
}