#!/usr/bin/env bash
set -euxo pipefail

rm -rf build
mkdir -p build
cd build
cmake .. "$@"
make -j"$(nproc)"
./test_macro_bsearch
./test_macro_cmp
./test_macro_heap
./test_macro_map
./test_macro_sort
./test_macro_time
./test_macro_to
cd ..
