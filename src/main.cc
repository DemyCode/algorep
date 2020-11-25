#include <fstream>
#include <iostream>
#include <mpi.h>
#include <cstdlib>

#include "mpi_classes/client.hh"
#include "mpi_classes/controller.hh"
#include "mpi_classes/node.hh"
#include "mpi_classes/process-information.hh"
#include "utils/string_utils.hh"

void exit()
{
    MPI_Finalize();
}

int main(int argc, char* argv[])
{
    if (argc <= 2)
    {
        std::cerr << "Usage: ./raft n_client n_server [path_command_list_client_1 path_command_list_client_2 ...]";
        return 1;
    }

    // Parse the arguments
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

    // Parse the command list files
    if (argc >= 4)
    {
        if (argc - 3 != n_client)
        {
            std::cerr << "Error, there should be one command list file per client" << std::endl;
            return 1;
        }

        // Check that the command list file is valid for each client
        for (int i = 0; i < n_client; i++)
        {
            std::ifstream ifile;
            ifile.open(argv[3 + i]);
            if (!ifile)
            {
                std::cerr << "Invalid path: " << argv[3 + i] << std::endl;
                return 1;
            }
            ifile.close();
        }
    }

    // Start the MPI instances
    int rank;
    int size;

    std::atexit(exit);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Set the different information in the singleton
    ProcessInformation::instance().set_information(rank, n_client, 1, n_node, n_client + 1, size);

    // Check that the size is coherent with the parameters
    if (size != n_node + n_client + 1)
    {
        std::cerr << "Usage: mpirun -np size ./raft n_client n_server [path_command_list_client_1 "
                     "path_command_list_client_2 ...]"
                  << std::endl
                  << "where size == n_client + n_server + 1" << std::endl;
        return 1;
    }

    // Controller
    if (rank == 0)
    {
        std::cout << "Start controller, rank: " << rank << std::endl;

        Controller controller;
        controller.run();
    }
    // Client
    else if (rank <= n_client)
    {
        Client client;

        // Add the command list files if they are present
        if (argc >= 4)
            client.add_command_list(argv[3 + rank - 1]);

        client.run();
    }
    // Server
    else
    {
        Node node;
        node.run();
    }

    return 0;
}