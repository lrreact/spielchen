#!/bin/bash

# Arena Shooter Client Launch Script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

if [ ! -f "$BUILD_DIR/arena-shooter" ]; then
    echo "Client executable not found. Building..."
    "$SCRIPT_DIR/build.sh"
fi

cd "$BUILD_DIR"
./arena-shooter "$@"
