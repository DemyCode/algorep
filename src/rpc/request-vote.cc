#include "request-vote.hh"

RequestVote::RequestVote(int term, size_t candidate_id, size_t last_log_index, int last_log_term)
    : RPC(term, RPC::RPC_TYPE::REQUEST_VOTE)
    , candidate_id_(candidate_id)
    , last_log_index_(last_log_index)
    , last_log_term_(last_log_term)
{}

RequestVote::RequestVote(int term, const json& serialized_json)
    : RequestVote(term,
                  serialized_json["candidate_id"],
                  serialized_json["last_log_index"],
                  serialized_json["last_log_term"])
{}

RequestVote::RequestVote(int term, const std::string& serialized)
    : RequestVote(term, json::parse(serialized))
{}

RPC::json RequestVote::serialize_message() const
{
    json json_object;

    json_object["candidate_id"] = this->candidate_id_;
    json_object["last_log_index"] = this->last_log_index_;
    json_object["last_log_term"] = this->last_log_term_;

    return json_object;
}

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

RPC::json RequestVoteResponse::serialize_message() const
{
    json json_object;

    json_object["vote_granted"] = this->vote_granted_;

    return json_object;
}