#include "request-vote-response.hh"

RequestVoteResponse::RequestVoteResponse(int term, bool vote_granted)
    : RPC(RPC::RPC_TYPE::REQUEST_VOTE_RESPONSE)
    , term_(term)
    , vote_granted_(vote_granted)
{}

RequestVoteResponse::RequestVoteResponse(const json& serialized_json)
    : RequestVoteResponse(serialized_json["term"], serialized_json["vote_granted"])
{}

RequestVoteResponse::RequestVoteResponse(const std::string& serialized)
    : RequestVoteResponse(json::parse(serialized))
{}

RequestVoteResponse::json RequestVoteResponse::serialize_message() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["vote_granted"] = this->vote_granted_;

    return json_object;
}