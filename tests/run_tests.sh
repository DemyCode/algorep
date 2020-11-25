#!/bin/sh

TIMEOUT=5

RED='\033[0;31m'
LIGHT_GREEN='\033[0;92m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
INVERT='\033[7m'
NC='\033[0m'


echo_fail()
{
    echo "${RED}${INVERT}[ FAIL  ]${NC}${INVERT} - ${test_name}${NC}"
}

run_integration_test()
{
    integration_folder="$1"
    test_name="$2"

    # Init basic var for the test
    test_file_input="${integration_folder}/${test_name}.in"
    test_file_output="${integration_folder}/${test_name}.out"
    test_file_log="${integration_folder}/${test_name}.log"

    client_count=$(sed '1q;d' "${test_file_input}")
    server_count=$(sed '2q;d' "${test_file_input}")
    generate_command_list=$(sed '3q;d' "${test_file_input}")
    commands=$(tail -n +4 "${test_file_input}")

    # Run test
    rm -f /tmp/raft_output
    echo "${commands}" | timeout "${TIMEOUT}" ./run.sh "${client_count}" "${server_count}" "${generate_command_list}" 2> /tmp/raft_output 1> /dev/null
    output="$?"

    # Exit if the test has timeout
    if [ "${output}" -eq 124 ]; then
        echo "${ORANGE}${INVERT}[TIMEOUT]${NC}${INVERT} - ${test_name}${NC}"
        return 1
    fi

    res=0
    print_fail=0

    # Check Output
    diff=$(diff -y --suppress-common-lines /tmp/raft_output "${test_file_output}")
    if [ -n "${diff}" ]; then
        res=1

        if [ "${print_fail}" -eq 0 ]; then
            print_fail=1
            echo_fail
        fi

        echo "       output"
        echo "${diff}"
    fi

    # Check Log
    if [ "${server_count}" -gt 0 ]; then
        test_file_log_sorted="${test_file_log}.sorted"

        if [ "${generate_command_list}" = "yes" ]; then
            # Create merged log file if command list is yes
            rm -f "${test_file_log_sorted}"

            for client_index in $(seq "${client_count}"); do
                client_log_file="/tmp/command_list_client_${client_index}.txt"
                cat "${client_log_file}" >> "${test_file_log_sorted}"
            done
        else
            cat "${test_file_log}" > "${test_file_log_sorted}"
        fi

        if [ "${client_count}" -gt 1 ]; then
            sort -o "${test_file_log_sorted}" "${test_file_log_sorted}"
        fi

        # For each server
        for server_rank in $(seq "$((client_count + 1))" "$((client_count + server_count))"); do
            log_file="log${server_rank}.txt"

            if [ -f "${log_file}" ]; then
                # If the log file has been created sort it
                log_file_sorted="${log_file}.sorted"

                if [ "${client_count}" -gt 1 ]; then
                    sort -o "${log_file_sorted}" "${log_file}"
                else
                    cat "${log_file}" > "${log_file_sorted}"
                fi

                # Check the log file with the sorted ref
                diff=$(diff -y --suppress-common-lines "${log_file_sorted}" "${test_file_log_sorted}" 2> /dev/null)
            else
                diff="Log file does not exist"
            fi

            # If the diff is not null
            if [ -n "${diff}" ]; then
                res=1

                if [ "${print_fail}" -eq 0 ]; then
                    print_fail=1
                    echo_fail
                fi

                echo "       log server ${server_rank}"
                echo "${diff}"
            fi

            # Clean log file for the server
            rm -f "${log_file}"
            rm -f "${log_file_sorted}"
        done

        # Clean sorted log file ref
        rm -rf "${test_file_log_sorted}"
    fi

    # If the test has succeeded
    if [ "${res}" -eq 0 ]; then
        echo "${GREEN}${INVERT}[  OK   ]${NC}${INVERT} - ${test_name}${NC}"
        res=0
    else
        echo ""
    fi

    return "${res}"
}

run_integration_tests()
{
    integration_folder="$1"
    integration_tests="$2"
    count=0
    fail=0

    # For each test in the integration folder
    for integration_test in ${integration_tests}; do
        run_integration_test "${integration_folder}" "${integration_test}"

        if [ "$?" -eq 1 ]; then
            fail=$((fail + 1))
        fi
        count=$((count + 1))
    done

    # Compute percentage of success
    recap="- $((count - fail))/${count} - $((100-(fail * 100) / count))%"
    if [ "${fail}" -eq 0 ]; then
        echo "${GREEN}[ OK ]${NC} ${recap}"
    elif [ "${fail}" -gt 0 ]; then
        echo "${RED}[FAIL]${NC} ${recap}"
    fi

    return "${fail}"
}

# Init basic var
current_dir="$(echo "$0" | rev | cut -f 2- -d '/' | rev)"
integration_folder="${current_dir}/integration"
integration_tests="$(find "${integration_folder}" -type f -a -name "*.in" -exec /bin/sh -c "echo {} | rev | cut -f 2- -d '.' | cut -f 1 -d '/' | rev" \; | sort)"

# Run integration tests
run_integration_tests "${integration_folder}" "${integration_tests}"
fail="$?"

exit "${fail}"