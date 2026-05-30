#!/bin/sh
file="dewlog"
dir="."

if [ -d "./build_debwithcov" ]; then dir="./build_debwithcov"
elif [ -d "./build_debwithasan" ]; then dir="./build_debwithasan"
elif [ -d "./build_debug" ]; then dir="./build_debug"
elif [ -d "./build_release" ]; then dir="./build_release"
else echo "No build directory found"; exit 1
fi

echo "Running tests in ${dir}"
( cd "${dir}" && ctest --output-on-failure ) || exit 1

if [ "${dir}" = "./build_debwithcov" ]; then
    echo "Generating coverage info"
    lcov --capture --directory "${dir}/tests/CMakeFiles" --output-file "${dir}/coverage.info" > /dev/null 2>&1 || exit 1
    lcov --extract "${dir}/coverage.info" "*/${file}.h" --output-file "${dir}/coverage.${file}.info" 2>/dev/null || exit 1
    genhtml "${dir}/coverage.${file}.info" --output-directory "./coverage_html" > /dev/null 2>/dev/null || exit 1
    rm "${dir}/coverage.info" || exit 1
fi
