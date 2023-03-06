#!/bin/bash -eu

for file in bee/*.cpp; do
  echo "Compiling $file..."
  clang++ -c $(cat compile_flags.txt) $file
done
