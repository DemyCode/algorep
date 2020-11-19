#include "client.hh"

Client::Client(int rank, int n_client, int offset, int size)
{
    this->rank = rank;
    this->n_client = n_client;
    this->offset = offset;
    this->size = size;
}

void Client::run()
{}
