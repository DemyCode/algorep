#include "request-vote-response.hh"

RequestVoteResponse::RequestVoteResponse(int term, bool vote_granted)
    : RPC(term, RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
    , vote_granted_(vote_granted)
{}

RequestVoteResponse::RequestVoteResponse(int term, const json& serialized_json)
    : RequestVoteResponse(term, serialized_json["vote_granted"].get<bool>())
{}

RequestVoteResponse::RequestVoteResponse(int term, const std::string& serialized)
    : RequestVoteResponse(term, json::parse(serialized))
{}

RequestVoteResponse::json RequestVoteResponse::serialize_message() const
{
    json json_object;

    json_object["vote_granted"] = this->vote_granted_;

    return json_object;
}