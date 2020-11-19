#include "request-vote-response.hh"

RequestVoteResponse::RequestVoteResponse(int term, bool vote_granted)
    : term_(term)
    , vote_granted_(vote_granted)
{}

RequestVoteResponse::RequestVoteResponse(const json& serialized_json)
    : term_(serialized_json["term"])
    , vote_granted_(serialized_json["vote_granted"])
{}

RequestVoteResponse::RequestVoteResponse(const std::string& serialized)
    : RequestVoteResponse(json::parse(serialized))
{}

std::string RequestVoteResponse::serialized() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["vote_granted"] = this->vote_granted_;

    return json_object.dump();
}