#!/bin/sh

# Run first
./tests $1 2>/dev/null 1>/dev/null

# If it failed, then rerun with stdout stderr
return_value=$?
if [ $return_value != "0" ]; then
    ./tests $1
    exit 1
fi

exit 0
