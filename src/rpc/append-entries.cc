#include "append-entries.hh"

#include <utility>

AppendEntries::AppendEntries(int term,
                             size_t leader_id,
                             int prev_log_index,
                             int prev_log_term,
                             std::vector<Entry> entries,
                             int leader_commit)
    : RPC(term, RPC::RPC_TYPE::APPEND_ENTRIES)
    , leader_id_(leader_id)
    , prev_log_index_(prev_log_index)
    , prev_log_term_(prev_log_term)
    , entries_(std::move(entries))
    , leader_commit_(leader_commit)
{}

AppendEntries::AppendEntries(int term, const json& serialized_json)
    : RPC(term, RPC::RPC_TYPE::APPEND_ENTRIES)
    , leader_id_(serialized_json["leader_id"])
    , prev_log_index_(serialized_json["prev_log_index"])
    , prev_log_term_(serialized_json["prev_log_term"])
    , entries_()
    , leader_commit_(serialized_json["leader_commit"])
{
    for (const auto& entry : serialized_json["entries"])
        this->entries_.emplace_back(entry);
}

AppendEntries::AppendEntries(int term, const std::string& serialized)
    : AppendEntries(term, json::parse(serialized))
{}

RPC::json AppendEntries::serialize_message() const
{
    json json_object;

    json_object["leader_id"] = this->leader_id_;
    json_object["prev_log_index"] = this->prev_log_index_;
    json_object["prev_log_term"] = this->prev_log_term_;

    json_object["entries"] = json::array();
    for (auto& entry : this->entries_)
        json_object["entries"].push_back(entry.serialize_message());

    json_object["leader_commit"] = this->leader_commit_;

    return json_object;
}

AppendEntriesResponse::AppendEntriesResponse(int term, bool success)
    : RPC(term, RPC::RPC_TYPE::APPEND_ENTRIES_RESPONSE)
    , success_(success)
{}

AppendEntriesResponse::AppendEntriesResponse(int term, const json& serialized_json)
    : AppendEntriesResponse(term, serialized_json["success"].get<bool>())
{}

AppendEntriesResponse::AppendEntriesResponse(int term, const std::string& serialized)
    : AppendEntriesResponse(term, json::parse(serialized))
{}

RPC::json AppendEntriesResponse::serialize_message() const
{
    json json_object;

    json_object["success"] = this->success_;

    return json_object;
}