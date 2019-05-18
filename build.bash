#!/bin/bash
clear

set -e

# Make build directory
mkdir -p build

# Build windows manually
if [[ "$OSTYPE" == "msys" ]]; then
    build_tests() {
        time                                      \
        $CXX -std=c++14 test/*.cpp                \
            "-Xclang -flto-visibility-public-std" \
            -Wall                                 \
            -Isrc                                 \
            -Imodules/variant/include             \
            -Imodules/test                        \
            -Imodules/optional                    \
            -Imodules/catch2                      \
            -o build/tests                        \
            >&2
        echo $?
    }

    build_samples() {
        time                                      \
        $CXX -std=c++14 sample/*.cpp              \
            "-Xclang -flto-visibility-public-std" \
            -Isrc                                 \
            -Imodules/variant/include             \
            -Imodules/optional                    \
            -Imodules/catch2                      \
            -o build/sample                       \
            >&2
        echo $?
    }

    return_a=$(build_tests &)
    return_b=$(build_samples &)
    wait
    if [[ $return_a != "0" || $return_b != "0" ]]; then
        echo "Failed builds"
        exit 1
    fi
# Build linux with CMake
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
    cd build
    cmake ..
    make -j
    cd ..
fi

####################
#     Run Tests    #
####################
set +e
./build/tests $1 2>/dev/null 1>/dev/null

# If it failed, then rerun with stdout stderr
return_value=$?
if [ $return_value != "0" ]; then
    ./build/tests $1
    exit 1
fi
echo "--------------------"
echo "| Tests Successful |"
echo "--------------------"
