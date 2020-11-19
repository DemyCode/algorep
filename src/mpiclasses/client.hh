//
// Created by mehdi on 18/11/2020.
//

#ifndef ALGOREP_CLIENT_HH
#define ALGOREP_CLIENT_HH


class Client {

public:
    Client(int rank, int n_client, int offset, int size);

    void run();

    int rank;
    int n_client;
    int offset;
    int size;
};


#endif //ALGOREP_CLIENT_HH
