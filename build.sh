#!/bin/sh
clear

set -e

build_tests() {
    time                                    \
    clang++ -std=c++14 test/*.cpp           \
        -Xclang -flto-visibility-public-std \
        -Wall                               \
        -Isrc                               \
        -Imodules/variant/include           \
        -Imodules/optional                  \
        -Imodules/catch2                    \
        -o tests.exe
    echo $?
}

build_samples() {
    time                                    \
    clang++ -std=c++14 sample/*.cpp         \
        -Xclang -flto-visibility-public-std \
        -Wall                               \
        -Isrc                               \
        -Imodules/variant/include           \
        -Imodules/optional                  \
        -Imodules/catch2                    \
        -o sample.exe
    echo $?
}

run_tests() {
    ./test_runner.sh
    echo $?
}

return_a=$(build_tests &)
return_b=$(build_samples &)
wait
if [[ $return_a != "0" || $return_b != "0" ]]; then
    echo "Failed builds"
    exit 1
fi

return_c=$(run_tests)
if [[ $return_c != "0" ]]; then
    echo "Failed tests"
    exit 1
fi
echo "Tests successful"
