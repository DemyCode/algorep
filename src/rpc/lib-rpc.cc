#include "lib-rpc.hh"

void send_message(const RPC& rpc_message, int destination, MPI_Request& request, int tag)
{
    const auto& serialized_message = rpc_message.serialize();
    MPI_Isend(
        serialized_message.c_str(), serialized_message.size(), MPI_CHAR, destination, tag, MPI_COMM_WORLD, &request);
}

RPCResponse receive_message(int source, int tag)
{
    int buffer_size = 0;
    MPI_Status mpi_status;

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
            return RPCResponse(message_type, RPCResponse::content_t(AppendEntries(message_content)));

        case RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE:
            return RPCResponse(message_type, RPCResponse::content_t(AppendEntriesResponse(message_content)));

        case RPC::RPC_TYPE::ENTRY:
            return RPCResponse(message_type, RPCResponse::content_t(Entry(message_content)));

        case RPC::RPC_TYPE::REQUEST_VOTE:
            return RPCResponse(message_type, RPCResponse::content_t(RequestVote(message_content)));

        case RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE:
            return RPCResponse(message_type, RPCResponse::content_t(RequestVoteResponse(message_content)));
    }
}