#!/bin/bash -eu

for file in bee/*.cpp; do
  echo "Compiling $file..."
  clang++ -c -std=c++20 -iquote . $file
done
