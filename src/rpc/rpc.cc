#include "rpc.hh"

RPC::RPC(RPC_TYPE rpc_type)
    : rpc_type_(rpc_type)
{}

std::string RPC::serialize() const
{
    json json_object;

    json_object["message_type"] = this->rpc_type_;
    json_object["message_content"] = this->serialize_message();

    return json_object.dump();
}
