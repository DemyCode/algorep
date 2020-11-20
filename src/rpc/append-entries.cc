#include "append-entries.hh"

#include <utility>

AppendEntries::AppendEntries(int term,
                             int leader_id,
                             int prev_log_index,
                             int prev_log_term,
                             std::vector<Entry> entries,
                             int leader_commit)
    : RPC(RPC::RPC_TYPE::APPEND_ENTRIES)
    , term_(term)
    , leader_id_(leader_id)
    , prev_log_index_(prev_log_index)
    , prev_log_term_(prev_log_term)
    , entries_(std::move(entries))
    , leader_commit_(leader_commit)
{}

AppendEntries::AppendEntries(const json& serialized_json)
    : RPC(RPC::RPC_TYPE::APPEND_ENTRIES)
    , term_(serialized_json["term"])
    , leader_id_(serialized_json["leader_id"])
    , prev_log_index_(serialized_json["prev_log_index"])
    , prev_log_term_(serialized_json["prev_log_term"])
    , entries_()
    , leader_commit_(serialized_json["leader_commit"])
{
    for (const auto& entry : serialized_json["entries"])
        this->entries_.emplace_back(entry);
}

AppendEntries::AppendEntries(const std::string& serialized)
    : AppendEntries(json::parse(serialized))
{}

AppendEntries::json AppendEntries::serialize_message() const
{
    json json_object;

    json_object["term"] = this->term_;
    json_object["leader_id"] = this->leader_id_;
    json_object["prev_log_index"] = this->prev_log_index_;
    json_object["prev_log_term"] = this->prev_log_term_;

    for (auto& entry : this->entries_)
        json_object["entries"].emplace_back(entry.serialize_message());

    json_object["leader_commit"] = this->leader_commit_;

    return json_object;
}
