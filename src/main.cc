#include <iostream>
#include <mpi.h>

#include "mpi_classes/client.hh"
#include "mpi_classes/controller.hh"
#include "mpi_classes/node.hh"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./raft n_client n_server";
        exit(1);
    }
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n_client = std::atoi(argv[1]);
    int n_node = std::atoi(argv[2]);
    if (size != n_node + n_client + 1)
    {
        std::cerr << "Usage: mpirun -np size ./raft n_client n_server" << std::endl;
        std::cerr << "where size == n_client + n_server + 1" << std::endl;
        exit(1);
    }

    if (rank == 0)
    {
        // CONSOLE
        Controller controller = Controller();
        controller.input();
    }
    else if (rank < n_client)
    {
        // CLIENT
        Client client = Client(rank, n_client, 1, size);
        client.run();
    }
    else
    {
        // SERVER
        int offset = n_client + 1;
        Node node = Node(rank, n_node, offset, size);
        node.run();
    }

    std::cout << "Hello, World, I am " << rank << " of " << size << "\n";
    MPI_Finalize();
    return 0;
}