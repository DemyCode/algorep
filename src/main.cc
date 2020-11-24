#include <experimental/filesystem>
#include <iostream>
#include <mpi.h>

#include "mpi_classes/client.hh"
#include "mpi_classes/controller.hh"
#include "mpi_classes/node.hh"
#include "mpi_classes/process-information.hh"
#include "utils/string_utils.hh"

int main(int argc, char* argv[])
{
    if (argc <= 2)
    {
        std::cerr << "Usage: ./raft n_client n_server [path_command_list_client_1 path_command_list_client_2 ...]";
        return 1;
    }

    int n_client = parse_int(argv[1]);
    int n_node = parse_int(argv[2]);

    if (n_client < 0)
    {
        std::cerr << "Invalid number of clients: " << argv[1] << std::endl;
        return 1;
    }

    if (n_node < 0)
    {
        std::cerr << "Invalid number of nodes: " << argv[2] << std::endl;
        return 1;
    }

    if (argc >= 4)
    {
        if (argc - 3 != n_client)
        {
            std::cerr << "Error, there should be one command list file per client" << std::endl;
            return 1;
        }

        for (int i = 0; i < n_client; i++)
        {
            if (!std::filesystem::is_regular_file(argv[3 + i]))
            {
                std::cerr << "Invalid path: " << argv[3 + i] << std::endl;
                return 1;
            }
        }
    }

    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ProcessInformation::instance().set_information(rank, n_client, 1, n_node, n_client + 1, size);

    if (size != n_node + n_client + 1)
    {
        std::cerr << "Usage: mpirun -np size ./raft n_client n_server [path_command_list_client_1 "
                     "path_command_list_client_2 ...]"
                  << std::endl
                  << "where size == n_client + n_server + 1" << std::endl;
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        std::cerr << "Start controller, rank: " << rank << std::endl;

        // CONSOLE
        Controller controller;
        controller.run();
    }
    else if (rank <= n_client)
    {
        std::cerr << "Start client, rank: " << rank << std::endl;

        // CLIENT
        Client client;

        if (argc >= 4)
            client.add_command_list(argv[3 + rank - 1]);

        client.run();
    }
    else
    {
        std::cerr << "Start server, rank: " << rank << std::endl;

        // SERVER
        int offset = n_client + 1;
        Node node(rank, n_node, offset, size);
        node.run();
    }

    MPI_Finalize();
    return 0;
}