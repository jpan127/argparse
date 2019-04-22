clear

SUPRESSED_CHECKS="\
-hicpp-avoid-c-arrays,\
-modernize-avoid-c-arrays,\
-hicpp-no-array-decay,\
-cppcoreguidelines-avoid-c-arrays,\
-cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
-fuchsia-default-arguments,\
-llvm-header-guard,\
-fuchsia-overloaded-operator,\
-misc-definitions-in-headers,\
-google-build-using-namespace\
"

clang-tidy.exe                 \
  -checks=*,$SUPRESSED_CHECKS \
  -header-filter=src/          \
  test/test.cpp                \
  --                           \
  -std=c++14                   \
  -Imodules/variant/include    \
  -Imodules/optional           \
  -Isrc                        \
  -Imodules/catch2
