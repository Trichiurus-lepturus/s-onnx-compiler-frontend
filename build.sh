#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT_DIR="/root/s-onnx"
BUILD_DIR="build"
CMAKE_BUILD_TYPE="Debug"
CMAKE_GENERATOR="Unix Makefiles"

if [ ! -d "${PROJECT_ROOT_DIR}/${BUILD_DIR}" ]; then
  echo "Creating build directory: ${BUILD_DIR}"
  mkdir -p "${PROJECT_ROOT_DIR}/${BUILD_DIR}"
fi

echo "Running CMake configuration..."
cmake \
  -S "${PROJECT_ROOT_DIR}" \
  -B "${PROJECT_ROOT_DIR}/${BUILD_DIR}" \
  -G "${CMAKE_GENERATOR}" \
  -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
  -DCMAKE_C_COMPILER=cc \
  -DCMAKE_CXX_COMPILER=c++ \


ln -sf "${PROJECT_ROOT_DIR}/${BUILD_DIR}/compile_commands.json" "${PROJECT_ROOT_DIR}/compile_commands.json"

echo "Starting compilation..."
cmake --build "${PROJECT_ROOT_DIR}/${BUILD_DIR}" -- -j"$(nproc)"

echo "Build completed successfully!"
