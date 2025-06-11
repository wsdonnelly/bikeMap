#include <napi.h>
#include <fstream>
#include <vector>
#include <cstdint>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

// Graph typedef
using Graph = boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::undirectedS,
  boost::no_property,
  boost::property<boost::edge_weight_t, float>
>;

static Graph g;
static std::vector<std::pair<float,float>> nodeCoords;

// Load CSR‐style binaries into Boost.Graph
void LoadGraph(const std::string& nodesPath, const std::string& edgesPath) {
  std::ifstream nin(nodesPath, std::ios::binary), ein(edgesPath, std::ios::binary);
  if (!nin && !ein)
  {
    throw std::runtime_error("Could not open nodes or edges file: " + nodesPath + edgesPath);
  }
  uint32_t N, M;
  nin.read(reinterpret_cast<char*>(&N), sizeof(N));
  nodeCoords.resize(N);
  for (uint32_t i = 0; i < N; ++i) {
    uint64_t id; float lat, lon;
    nin.read(reinterpret_cast<char*>(&id), sizeof(id));
    nin.read(reinterpret_cast<char*>(&lat), sizeof(lat));
    nin.read(reinterpret_cast<char*>(&lon), sizeof(lon));
    nodeCoords[i] = {lat, lon};
  }
  ein.read(reinterpret_cast<char*>(&N), sizeof(N));
  ein.read(reinterpret_cast<char*>(&M), sizeof(M));
  std::vector<uint32_t> offsets(N+1);
  for (uint32_t i = 0; i <= N; ++i) {
    ein.read(reinterpret_cast<char*>(&offsets[i]), 4);
  }
  std::vector<uint32_t> nbrs(M);
  std::vector<float>    wts(M);
  for (uint32_t i = 0; i < M; ++i) ein.read(reinterpret_cast<char*>(&nbrs[i]), 4);
  for (uint32_t i = 0; i < M; ++i) ein.read(reinterpret_cast<char*>(&wts[i]), 4);

  g = Graph(N);
  for (uint32_t u = 0; u < N; ++u) {
    for (uint32_t ei = offsets[u]; ei < offsets[u+1]; ++ei) {
      boost::add_edge(u, nbrs[ei], wts[ei], g);
    }
  }
}

Napi::Array FindPath(const Napi::CallbackInfo& info) {
  uint32_t start = info[0].As<Napi::Number>().Uint32Value();
  uint32_t end   = info[1].As<Napi::Number>().Uint32Value();

  std::vector<float> dist(num_vertices(g));
  std::vector<uint32_t> pred(num_vertices(g));
  boost::dijkstra_shortest_paths(
    g,
    start,
    boost::predecessor_map(&pred[0]).distance_map(&dist[0])
  );

  // Reconstruct path
  std::vector<uint32_t> path;
  for (uint32_t v = end; v != start; v = pred[v]) {
    path.push_back(v);
  }
  path.push_back(start);
  std::reverse(path.begin(), path.end());

  Napi::Env env = info.Env();
  Napi::Array jsPath = Napi::Array::New(env, path.size());
  for (size_t i = 0; i < path.size(); ++i) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("nodeIdx", path[i]);
    obj.Set("lat",  nodeCoords[path[i]].first);
    obj.Set("lon",  nodeCoords[path[i]].second);
    jsPath.Set(i, obj);
  }
  return jsPath;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  LoadGraph("../data/graph_nodes.bin", "../data/graph_edges.bin");
  exports.Set("findPath", Napi::Function::New(env, FindPath));
  return exports;
}

NODE_API_MODULE(route, Init);
