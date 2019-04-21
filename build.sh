clear

time                            \
g++ -std=c++14 test/*.cpp       \
    -Isrc                       \
    -Imodules/variant/include   \
    -Imodules/optional          \
    -Imodules/catch2            \
    -o tests.exe
