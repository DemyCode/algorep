#!/bin/sh

RED='\033[0;31m'
LIGHT_GREEN='\033[0;92m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
INVERT='\033[7m'
NC='\033[0m'


run_integration_test()
{
    integration_folder="$1"
    test_name="$2"

    test_file_input="${integration_folder}/${test_name}.in"
    test_file_output="${integration_folder}/${test_name}.in"

    client_count=$(sed '1q;d' "${test_file_input}")
    server_count=$(sed '2q;d' "${test_file_input}")
    generate_command_list=$(sed '3q;d' "${test_file_input}")
    commands=$(tail -n +4 "${test_file_input}")

    echo "client_count: ${client_count}"
    echo "server_count: ${server_count}"
    echo "generate_command_list: ${generate_command_list}"
    echo "commands: ${commands}"
    
    # TODO RUN TEST

    res=0

    if [ "${res}" -eq 1 ]; then
        echo "${RED}${INVERT}[FAIL]${NC}${INVERT} - ${test_name}${NC}"
        echo "       stdout > ${stdout}"
        echo "${diff}"
        echo "       stderr > ${stderr} (${l1} - ${l2})"
        echo "${diff_err}"
        echo "       output > ${output} (${o1} - ${o2})"
        echo ""
    else
        echo "${GREEN}${INVERT}[ OK ]${NC}${INVERT} - ${test_name}${NC}"
    fi

    return "$res"

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
}

current_dir="$(echo "$0" | rev | cut -f 2- -d '/' | rev)"
integration_folder="${current_dir}/integration"
integration_tests="$(find "${integration_folder}" -type f -a -name "*.in" -exec /bin/sh -c "echo {} | rev | cut -f 2- -d '.' | cut -f 1 -d '/' | rev" \; | sort)"

run_integration_tests "${integration_folder}" "${integration_tests}"