#!/usr/bin/env bash
set -euo pipefail

echo "Removing build/ directory…"
rm -rf build

echo "Removing any generated binary files…"
rm -f ../data/graph_nodes.bin ../data/graph_edges.bin ../data/kd_nodes.bin ../data/.DS_Store

echo "Clean complete."
