#!/bin/bash

set -e

SUPRESSED_CHECKS="\
-cppcoreguidelines-avoid-c-arrays,\
-cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
-cppcoreguidelines-pro-bounds-constant-array-index,\
-cppcoreguidelines-pro-bounds-pointer-arithmetic,\
-cppcoreguidelines-pro-type-union-access,\
-cppcoreguidelines-pro-type-vararg,\
-fuchsia-default-arguments,\
-fuchsia-overloaded-operator,\
-google-build-using-namespace,\
-google-runtime-references,\
-hicpp-avoid-c-arrays,\
-hicpp-no-array-decay,\
-hicpp-vararg,\
-llvm-header-guard,\
-misc-definitions-in-headers,\
-modernize-avoid-c-arrays,\
-readability-avoid-const-params-in-decls,\
-readability-named-parameter\
"

if [[ "$OSTYPE" == "msys" ]]; then
    CLANG_TIDY_PATH="clang-tidy.exe"
    STDLIB=""
    EXTRA_FLAGS="-Xclang -flto-visibility-public-std"
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
    STDLIB="-stdlib=libc++"
    CLANG_TIDY_PATH="clang-tidy-8"
    EXTRA_FLAGS=""
fi

# Compiler flags
FLAGS="-std=c++14 $STDLIB $EXTRA_FLAGS -Imodules/optional -Iargparse/include -Imodules/catch2"

# Run clang-tidy
python3 runtidy.py                    \
    --checks=*,$SUPRESSED_CHECKS      \
    --header-filter=argparse/include/ \
    --cxxflags="$FLAGS"               \
    argparse/src/*.cpp

echo "--------------------"
echo "|    All Tidied    |"
echo "--------------------"
