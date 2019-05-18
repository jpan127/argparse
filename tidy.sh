#!/bin/sh

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

$CLANG_TIDY_PATH                              \
    -checks=*,$SUPRESSED_CHECKS               \
    --warnings-as-errors=*,$SUPRESSED_CHECKS  \
    -quiet                                    \
    -header-filter=argparse/include/          \
    argparse/src/*.cpp                        \
    --                                        \
    -std=c++14                                \
    $STDLIB                                   \
    $EXTRA_FLAGS                              \
    -Imodules/variant/include                 \
    -Imodules/optional                        \
    -Iargparse/include                        \
    -Imodules/catch2

echo "--------------------"
echo "|    All Tidied    |"
echo "--------------------"
