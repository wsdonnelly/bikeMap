#!/usr/bin/env bash
set -euo pipefail

# Remove any existing build directory
if [ -d build ]; then
  echo "Removing existing build/ directory..."
  rm -rf build
fi

# Create a fresh build directory
echo "Creating build/ directory..."
mkdir build
cd build

# Configure and build
echo "Running cmake …"
cmake ..
echo "Running make …"
make

echo "Build complete."
