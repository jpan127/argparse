#!/bin/sh
./tests.exe 2>/dev/null 1>/dev/null

# If it failed, then rerun with stdout stderr
return_value=$?
if [ $return_value != "0" ]; then
    ./tests.exe
    exit 1
fi

exit 0
