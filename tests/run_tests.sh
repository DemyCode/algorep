#!/bin/sh

TIMEOUT=1

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

    test_file_input="${integration_folder}/${test_name}.in"
    test_file_output="${integration_folder}/${test_name}.out"
    test_file_log="${integration_folder}/${test_name}.log"

    client_count=$(sed '1q;d' "${test_file_input}")
    server_count=$(sed '2q;d' "${test_file_input}")
    generate_command_list=$(sed '3q;d' "${test_file_input}")
    commands=$(tail -n +4 "${test_file_input}")

    echo "${commands}" | timeout "${TIMEOUT}" ./run.sh "${client_count}" "${server_count}" ${generate_command_list} 2> /tmp/raft_output 1> /dev/null
    output="$?"

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
        for server_rank in $(seq "$((client_count + 1))" "$((client_count + server_count))"); do
            log_file="log${server_rank}.txt"

            diff=$(diff -y --suppress-common-lines "${log_file}" "${test_file_log}")

            if [ -n "${diff}" ]; then
                res=1

                if [ "${print_fail}" -eq 0 ]; then
                    print_fail=1
                    echo_fail
                fi

                echo "       log server ${server_rank}"
                echo "${diff}"
            fi

            rm -f "${log_file}"
        done
    fi

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

    for integration_test in ${integration_tests}; do
        run_integration_test "${integration_folder}" "${integration_test}"

        if [ "$?" -eq 1 ]; then
            fail=$((fail + 1))
        fi
        count=$((count + 1))
    done

    recap="- $((count - fail))/${count} - $((100-(fail * 100) / count))%"
    if [ "${fail}" -eq 0 ]; then
        echo "${GREEN}[ OK ]${NC} ${recap}"
    elif [ "${fail}" -gt 0 ]; then
        echo "${RED}[FAIL]${NC} ${recap}"
    fi

    return "${fail}"
}

current_dir="$(echo "$0" | rev | cut -f 2- -d '/' | rev)"
integration_folder="${current_dir}/integration"
integration_tests="$(find "${integration_folder}" -type f -a -name "*.in" -exec /bin/sh -c "echo {} | rev | cut -f 2- -d '.' | cut -f 1 -d '/' | rev" \; | sort)"

run_integration_tests "${integration_folder}" "${integration_tests}"
fail="$?"

rm -f /tmp/raft_output

exit "${fail}"