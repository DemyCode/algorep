#include "request-vote.hh"

RequestVote::RequestVote(int term, int candidate_id, int last_log_index, int last_log_term)
    : term_(term)
    , candidate_id_(candidate_id)
    , last_log_index_(last_log_index)
    , last_log_term_(last_log_term)
{}

RequestVote::RequestVote(const json& serialized_json)
    : term_(serialized_json["term"])
    , candidate_id_(serialized_json["candidate_id"])
    , last_log_index_(serialized_json["last_log_index"])
    , last_log_term_(serialized_json["last_log_term"])
{}

RequestVote::RequestVote(const std::string& serialized)
    : RequestVote(json::parse(serialized))
{}

std::string RequestVote::serialize() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["candidate_id"] = this->candidate_id_;
    json_object["last_log_index"] = this->last_log_index_;
    json_object["last_log_term"] = this->last_log_term_;

    return json_object.dump();
}
