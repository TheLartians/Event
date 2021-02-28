#! /bin/bash

CURDIR=$(/bin/pwd)
# GENERATOR="Unix Makefiles"
GENERATOR=Ninja

set -u
set -e
set -x

STAGEDIR="${CURDIR}/stage"
CMAKE_PRESET="-G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=${STAGEDIR}"

export CPM_SOURCE_CACHE=${HOME}/.cache/CPM
export CPM_USE_LOCAL_PACKAGES=0

export CXX=g++-10

# distclean
rm -rf build root stage stagedir

# update CPM.cmake
#XXX wget -q -O cmake/CPM.cmake https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake

# install the library
cmake -S . -B build/install ${CMAKE_PRESET} -DCMAKE_INSTALL_PREFIX=${STAGEDIR} -DCMAKE_CXX_STANDARD=20 # --trace-expand
cmake --build build/install --target install

# test the library
cmake -S test -B build/test ${CMAKE_PRESET} -DTEST_INSTALLED_VERSION=1 #NO! -DENABLE_TEST_COVERAGE=1
cmake --build build/test
cmake --build build/test --target test

# all together
# cmake -S all -B build/all ${CMAKE_PRESET} -DTEST_INSTALLED_VERSION=1 -DENABLE_TEST_COVERAGE=1
# cmake --build build/all
# cmake --build build/all --target test
# cmake --build build/all --target check-format

# check the library
cmake -S . -B build/example ${CMAKE_PRESET} -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build build/example --target all
run-clang-tidy.py -p build/example -checks='-*,modernize-*,misc-*,hicpp-*,cert-*,readability-*,portability-*,performance-*,cppcore*' example
