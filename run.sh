#!/bin/sh

# Define constants
BUILD_DIRECTORY_LIST=("build" "cmake-build-release" "cmake-build-debug")
EXEC_NAME="raft"

print_usage()
{
    if [ $# -eq 1 ]; then
        echo "ERROR: $1"
    fi

    echo "USAGE: ./run.sh client_count server_count [build_path]"
    exit 1
}

# Check the number of arguments
if [ $# -lt 2 ]; then
    print_usage "Invalid number of arguments"
fi

number_regex="^[0-9]+$"

# Check if the first and second arguments are numbers
if ! echo "$1" | grep -E "${number_regex}" > /dev/null; then
    print_usage "First argument must be a number"
fi

if ! echo "$2" | grep -E "${number_regex}" > /dev/null; then
    print_usage "Second argument must be a number"
fi

client_count="$1"
server_count="$2"
total_size=$((client_count + server_count + 1))

# Check the build path (location of the executable)
build_path="$3"
if [ $# -lt 3 ]; then
    echo "No build directory given, looking for one ..."

    for path in "${BUILD_DIRECTORY_LIST[@]}"; do
        if [ -d "${path}" ] && [ -x "${path}/${EXEC_NAME}" ]; then
            build_path="${path}"
            echo "Found path ${path}"
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

echo "--------------------------------------------------------------------------"
echo "Running with the following parameters:"
echo "Client count: ${client_count}"
echo "Server count: ${server_count}"
echo ""

mpirun -np ${total_size} ./"${build_path}/${EXEC_NAME}" "${client_count}" "${server_count}"