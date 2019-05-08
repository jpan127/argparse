#!/bin/sh

# Set executable path
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    TESTS_EXECUTABLES="./tests"
elif [[ "$OSTYPE" == "msys" ]]; then
    TESTS_EXECUTABLES="./tests.exe"
fi

# Run first
$TESTS_EXECUTABLES $1 2>/dev/null 1>/dev/null

# If it failed, then rerun with stdout stderr
return_value=$?
if [ $return_value != "0" ]; then
    $TESTS_EXECUTABLES $1
    exit 1
fi

exit 0
