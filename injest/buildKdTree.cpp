// src/ingest/buildKdTree.cpp
#include <fstream>
#include <vector>
#include <cstdint>

// Simple on-disk format: [uint32 N] [ float32 lat_0, float32 lon_0, uint32 idx_0 ] … repeated
int main() {
  std::ifstream in("data/graph_nodes.bin", std::ios::binary);
  if (!in) {
    std::cerr << "Error: graph_nodes.bin not found\n";
    return 1;
  }
  uint32_t N;
  in.read(reinterpret_cast<char*>(&N), sizeof(N));
  // Skip past N × (uint64 + float + float):
  in.seekg(sizeof(uint32_t) + N*(sizeof(uint64_t)+sizeof(float)+sizeof(float)));

  // Rewind and re-read in a way that’s easy: actually re-open so we avoid manual seek confusion:
  in.close();
  in.open("data/graph_nodes.bin", std::ios::binary);
  uint32_t n_check; in.read(reinterpret_cast<char*>(&n_check), sizeof(n_check));
  // We know n_check == N
  std::vector<std::tuple<float,float,uint32_t>> kdData;
  kdData.reserve(N);
  for (uint32_t i = 0; i < N; ++i) {
    uint64_t nodeId; float lat, lon;
    in.read(reinterpret_cast<char*>(&nodeId), sizeof(nodeId));
    in.read(reinterpret_cast<char*>(&lat), sizeof(lat));
    in.read(reinterpret_cast<char*>(&lon), sizeof(lon));
    kdData.emplace_back(lat, lon, i); // i = nodeIdx (0..N-1)
  }
  in.close();

  std::ofstream out("data/kd_nodes.bin", std::ios::binary);
  out.write(reinterpret_cast<const char*>(&N), sizeof(N));
  for (auto& [lat, lon, idx] : kdData) {
    out.write(reinterpret_cast<const char*>(&lat), sizeof(lat));
    out.write(reinterpret_cast<const char*>(&lon), sizeof(lon));
    out.write(reinterpret_cast<const char*>(&idx), sizeof(idx));
  }
  out.close();
  std::cout << "Wrote kd_nodes.bin (" << N << " points)\n";
  return 0;
}
