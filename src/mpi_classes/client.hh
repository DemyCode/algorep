#pragma once

class Client
{
public:
    Client(int rank, int n_client, int offset, int size);

    void run();

    int rank;
    int n_client;
    int offset;
    int size;
};
