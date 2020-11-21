#include "rpc.hh"

RPC::RPC(int term, RPC_TYPE rpc_type)
    : term_(term)
    , rpc_type_(rpc_type)
{}

std::string RPC::serialize() const
{
    json json_object;

    json_object["message_type"] = this->rpc_type_;
    json_object["term"] = this->term_;
    json_object["message_content"] = this->serialize_message();

    return json_object.dump();
}
