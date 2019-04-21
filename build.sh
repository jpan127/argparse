clear

set -e

time                            \
g++ -std=c++14 test/*.cpp       \
    -Wall                       \
    -Isrc                       \
    -Imodules/variant/include   \
    -Imodules/optional          \
    -Imodules/catch2            \
    -o tests.exe

echo "Build successful"
