#!/usr/bin/env bash
set -e

echo "Initializing submodules..."
git submodule update --init --recursive

echo "Configuring..."
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "Building..."
cmake --build build -j"$(nproc)"

echo "Linking compile_commands.json..."
ln -sf build/compile_commands.json compile_commands.json

echo
echo "Build successful!"
echo "Executable: ./build/cdsl"

