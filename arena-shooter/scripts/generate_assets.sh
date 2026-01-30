#!/bin/bash

# Arena Shooter Asset Generation Script
# Converts OpenSCAD files to OBJ format
# Requires: openscad

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
MODELS_DIR="$PROJECT_DIR/assets/models"

echo "=================================="
echo "  Asset Generation Script"
echo "=================================="
echo "Models directory: $MODELS_DIR"
echo ""

# Check if OpenSCAD is installed
if ! command -v openscad &> /dev/null; then
    echo "Warning: OpenSCAD not found. Please install it to generate 3D assets."
    echo "  Ubuntu/Debian: sudo apt install openscad"
    echo "  macOS: brew install openscad"
    echo "  Windows: Download from https://openscad.org/downloads.html"
    echo ""
    echo "Skipping asset generation..."
    exit 0
fi

# Process all .scad files
cd "$MODELS_DIR"
SCAD_FILES=$(ls *.scad 2>/dev/null || true)

if [ -z "$SCAD_FILES" ]; then
    echo "No .scad files found in $MODELS_DIR"
    exit 0
fi

for scad_file in $SCAD_FILES; do
    obj_file="${scad_file%.scad}.obj"
    echo "Converting $scad_file -> $obj_file"
    openscad -o "$obj_file" "$scad_file"
done

echo ""
echo "=================================="
echo "  Asset generation complete!"
echo "=================================="
