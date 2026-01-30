#!/bin/bash

# Arena Shooter Dedicated Server Launch Script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

PORT="${1:-7777}"
MAX_PLAYERS="${2:-16}"

if [ ! -f "$BUILD_DIR/arena-shooter-server" ]; then
    echo "Server executable not found. Building..."
    "$SCRIPT_DIR/build.sh"
fi

echo "Starting server on port $PORT with max $MAX_PLAYERS players..."
"$BUILD_DIR/arena-shooter-server" --port "$PORT" --max-players "$MAX_PLAYERS"
