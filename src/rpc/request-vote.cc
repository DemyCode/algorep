#include "request-vote.hh"

RequestVote::RequestVote(int term, int candidate_id, int last_log_index, int last_log_term)
    : RPC(RPC::RPC_TYPE::REQUEST_VOTE)
    , term_(term)
    , candidate_id_(candidate_id)
    , last_log_index_(last_log_index)
    , last_log_term_(last_log_term)
{}

RequestVote::RequestVote(const json& serialized_json)
    : RequestVote(serialized_json["term"],
                  serialized_json["candidate_id"],
                  serialized_json["last_log_index"],
                  serialized_json["last_log_term"])
{}

RequestVote::RequestVote(const std::string& serialized)
    : RequestVote(json::parse(serialized))
{}

RequestVote::json RequestVote::serialize_message() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["candidate_id"] = this->candidate_id_;
    json_object["last_log_index"] = this->last_log_index_;
    json_object["last_log_term"] = this->last_log_term_;

    return json_object;
}
