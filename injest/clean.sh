#!/usr/bin/env bash
set -euo pipefail

echo "Removing build/ directory…"
rm -rf build

echo "Removing any generated binary files…"
rm -f graph_nodes.bin graph_edges.bin kd_nodes.bin

echo "Clean complete."
