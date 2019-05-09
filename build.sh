#!/bin/sh
clear

set -e

if [[ -z ${CXX+x} ]]; then
    CXX="clang++"
fi

if [[ "$OSTYPE" == "msys" ]]; then
    STDLIB=""
    EXTRA_FLAGS="-Xclang -flto-visibility-public-std"
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
    STDLIB="-stdlib=libc++"
    EXTRA_FLAGS=""
fi

build_tests() {
    time                                    \
    $CXX -std=c++14 test/*.cpp              \
        $STDLIB                             \
        $EXTRA_FLAGS                        \
        -Wall                               \
        -Isrc                               \
        -Imodules/variant/include           \
        -Imodules/test                      \
        -Imodules/optional                  \
        -Imodules/catch2                    \
        -o tests                            \
        >&2
    echo $?
}

build_samples() {
    time                                    \
    $CXX -std=c++14 sample/*.cpp            \
        $STDLIB                             \
        $EXTRA_FLAGS                        \
        -Isrc                               \
        -Imodules/variant/include           \
        -Imodules/optional                  \
        -Imodules/catch2                    \
        -o sample.exe                       \
        >&2
    echo $?
}

run_tests() {
    bash ./test_runner.sh $1 >&2
    echo $?
}

return_a=$(build_tests &)
return_b=$(build_samples &)
wait
if [[ $return_a != "0" || $return_b != "0" ]]; then
    echo "Failed builds"
    exit 1
fi

return_c=$(run_tests $1)
if [[ $return_c != "0" ]]; then
    echo "Failed tests"
    exit 1
fi
echo "Tests successful"

bash ./tidy.sh
echo "All tidied!"
