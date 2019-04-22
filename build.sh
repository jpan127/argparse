clear

set -e

time                                    \
clang++ -std=c++14 test/*.cpp           \
    -Xclang -flto-visibility-public-std \
    -Wall                               \
    -Isrc                               \
    -Imodules/variant/include           \
    -Imodules/optional                  \
    -Imodules/catch2                    \
    -o tests.exe

echo "Build successful"
