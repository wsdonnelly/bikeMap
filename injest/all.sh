#!/usr/bin/env bash
set -euo pipefail

# 1) Clean previous artifacts
./clean.sh

# 2) Build everything
./build.sh

# 3) Run buildGraph (you’ll need to supply the OSM file path)
if [ $# -ne 1 ]; then
  echo "Usage: $0 path/to/helsinki-latest.osm.pbf"
  exit 1
fi
OSM_PBF="$1"
echo "Running buildGraph …"
./run-buildGraph.sh "${OSM_PBF}"

# 4) Run buildKdTree
echo "Running buildKdTree …"
./run-buildKdTree.sh

echo "All steps complete."
