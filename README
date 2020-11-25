# RAFT Consensus Algorithm for Log Replication
Projec for the Distributed Algorithm class at EPITA.

Authors:
- Mehdi Bekhtaoui
- Axen Georget
- Brieg Oudea
- Mickael Gandon

Supervised by:
- Etienne Renault

## 1. Requirements
To use this project you should install MPI, to do so you should install the `libopenmpi-dev` package.
Along with that this project requires cmake and a C++ compiler supporting the C++17 version.
This project also uses the tool make.

## 2. Build
In order to build this project you can run the following command, it will create a directory call build and add the executable inside:
````shell script
make
````

If you want to create a debug environment you can run the following command:
````shell script
make debug
````

## 3. Run the Program
To run the program you can use the `run.sh` script by executing the following commands:
```
chmod +x run.sh
./run.sh [client_count] [server_count] [generate_command_list]
```

Here is a quick description of the parameters:
- `client_count`: Number of client instances to create
- `server_count`: Number of client instances to create
- `generate_command_list`: (`yes` or `no`) if `yes` it will generate a random command list which will be given to the clients

The program will create one file per server containing the replicated log. Those files are named `log[rank_of_server].txt`.

### 3.1 Controller Commands

When running you can interact with the controller, here is the list and descriptions of the available commands:
- `help`: print the help message
- `list_ranks`: print the list of ranks with the type of the process associated
- `entry [client_rank] [command]`: send an entry to the client_rank
- `start [client_rank]`: start the client_rank
- `wait_finish [client_rank]`: wait that a client has send all his messages
- `timeout [server_rank]`: simulate an election timeout on the server_rank (force to be candidate)
- `get_state [server_rank]`: get the state of the server_rank
- `print_local_log [server_rank]`: print the local log of the server_rank
- `set_speed [process_rank] [speed]`: set the speed of the process_rank, speed available: {low, medium, high}
- `crash [process_rank]`: crash the process_rank
- `recover [process_rank]`: recover the process_rank
- `stop [process_rank]`: stop the process_rank
- `sleep [milliseconds]`: sleep the current process for milliseconds

## 4. Run the Test Suite
In order to run the test suite you can run the following command:
```shell script
make test
``` 

## 5. Debug the Program
When building the program in debug mode, it will automatically create one debug file per server instance.
Those files are located in the folder `debug`. You can also use the python script `tests/merge_log.py` this way:
```shell script
python3 tests/merge_log.py
```

This script will create a file `debug/merged.txt` containing all the debug log merged and sorted by timestamp.