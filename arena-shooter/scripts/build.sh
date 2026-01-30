#!/bin/bash

# Arena Shooter Build Script
# Usage: ./scripts/build.sh [debug|release]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

BUILD_TYPE="${1:-Release}"

# Normalize build type
case "${BUILD_TYPE,,}" in
    debug)
        BUILD_TYPE="Debug"
        ;;
    release)
        BUILD_TYPE="Release"
        ;;
    *)
        BUILD_TYPE="Release"
        ;;
esac

echo "=================================="
echo "  Arena Shooter Build Script"
echo "=================================="
echo "Build type: $BUILD_TYPE"
echo "Project dir: $PROJECT_DIR"
echo ""

# Create build directory
BUILD_DIR="$PROJECT_DIR/build"
mkdir -p "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..

# Build
echo ""
echo "Building..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . --parallel "$NPROC"

echo ""
echo "=================================="
echo "  Build complete!"
echo "=================================="
echo "Executables:"
echo "  - $BUILD_DIR/arena-shooter (client)"
echo "  - $BUILD_DIR/arena-shooter-server (dedicated server)"
echo ""
echo "Run with:"
echo "  cd $BUILD_DIR && ./arena-shooter"
