#!/bin/sh

# Define constants
BUILD_DIRECTORY_LIST="build cmake-build-release cmake-build-debug"
EXEC_NAME="raft"
MIN_COMMAND_COUNT=10
MAX_COMMAND_COUNT=100

echo_error()
{
    >&2 echo "$@"
}

print_usage()
{
    if [ $# -eq 1 ]; then
        echo "ERROR: $1"
    fi

    echo_error "USAGE: ./run.sh client_count server_count generate_command_list [build_path]"
    echo_error "  client_count: (int) number of client to create"
    echo_error "  server_count: (int) number of server to create"
    echo_error "  generate_command_list: (yes or no) if yes will generate a command list file for each client"
    echo_error "  build_path: (path, optional) path to the directory containing the executable"
    exit 1
}

generate_command_list_file()
{
    client_index="$1"
    file_name="/tmp/command_list_client_${client_index}.txt"

    rm "${file_name}"

    command_count="$(shuf -i ${MIN_COMMAND_COUNT}-${MAX_COMMAND_COUNT} -n 1)"

    for command_index in $(seq "${command_count}"); do
        random_number="$(shuf -i 1-10000 -n 1)"
        command="command_test ${random_number}"
        echo "${command}" >> "${file_name}"
    done
}

generate_command_list_files()
{
    client_count="$1"

    for client_index in $(seq "${client_count}"); do
        generate_command_list_file "${client_index}"
    done
}

# Check the number of arguments
if [ $# -lt 3 ]; then
    print_usage "Invalid number of arguments"
fi

client_count="$1"
server_count="$2"
generate_command_list="$3"
build_path="$4"

number_regex="^[0-9]+$"

# Check if the first and second arguments are numbers
if ! echo "${client_count}" | grep -E "${number_regex}" > /dev/null; then
    print_usage "First argument must be a number"
fi

if ! echo "${server_count}" | grep -E "${number_regex}" > /dev/null; then
    print_usage "Second argument must be a number"
fi

total_size=$((client_count + server_count + 1))

# Check the generate_command_list
if [ "${generate_command_list}" != "yes" ] && [ "${generate_command_list}" != "no" ]; then
    print_usage "Third argument must be 'yes' or 'no'"
fi

# Check the build path (location of the executable)
if [ $# -lt 4 ]; then
    echo_error "No build directory given, looking for one ..."

    for path in $BUILD_DIRECTORY_LIST; do
        if [ -d "${path}" ] && [ -x "${path}/${EXEC_NAME}" ]; then
            build_path="${path}"
            echo_error "Found path ${path}"
            break
        fi
    done

    if [ -z "${build_path}" ]; then
        print_usage "No build path were found"
    fi
fi

# Check if the executable is inside the build path
if ! [ -x "${path}/${EXEC_NAME}" ]; then
    print_usage "No executable found in the build path"
fi

# Generate command list files if needed
command_list_files=""
if [ "${generate_command_list}" = "yes" ] && [ "${client_count}" -gt 0 ]; then
    generate_command_list_files "${client_count}"

    for client_index in $(seq "${client_count}"); do
         command_list_files="${command_list_files} /tmp/command_list_client_${client_index}.txt"
    done
fi

echo_error "--------------------------------------------------------------------------"
echo_error "Running with the following parameters:"
echo_error "Client count: ${client_count}"
echo_error "Server count: ${server_count}"
echo_error "Command list: ${generate_command_list}"
if [ "${generate_command_list}" = "yes" ]; then
    echo_error "Command list files:${command_list_files}"
fi
echo_error ""

mpirun -np ${total_size} ./"${build_path}/${EXEC_NAME}" "${client_count}" "${server_count}" ${command_list_files}

for command_list_file in ${command_list_files}; do
    rm "${command_list_file}"
done