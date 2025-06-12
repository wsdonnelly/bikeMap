#include <napi.h>
#include <fstream>
#include <iostream>
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

// Napi::Array FindPath(const Napi::CallbackInfo& info) {
//   uint32_t start = info[0].As<Napi::Number>().Uint32Value();
//   uint32_t end   = info[1].As<Napi::Number>().Uint32Value();

//   std::vector<float> dist(num_vertices(g));
//   std::vector<uint32_t> pred(num_vertices(g));
//   boost::dijkstra_shortest_paths(
//     g,
//     start,
//     boost::predecessor_map(&pred[0]).distance_map(&dist[0])
//   );

//   // Reconstruct path
//   std::vector<uint32_t> path;
//   for (uint32_t v = end; v != start; v = pred[v]) {
//     path.push_back(v);
//   }
//   path.push_back(start);
//   std::reverse(path.begin(), path.end());

//   Napi::Env env = info.Env();
//   Napi::Array jsPath = Napi::Array::New(env, path.size());
//   for (size_t i = 0; i < path.size(); ++i) {
//     Napi::Object obj = Napi::Object::New(env);
//     obj.Set("nodeIdx", path[i]);
//     obj.Set("lat",  nodeCoords[path[i]].first);
//     obj.Set("lon",  nodeCoords[path[i]].second);
//     jsPath.Set(i, obj);
//   }
//   return jsPath;
// }

// Napi::Value FindPath(const Napi::CallbackInfo& info) {
//   Napi::Env env = info.Env();

//   // 1) Validate arguments
//   if (info.Length() < 2 ||
//       !info[0].IsNumber() ||
//       !info[1].IsNumber()) {
//     Napi::TypeError::New(env, "Expected (startIdx: number, endIdx: number)")
//         .ThrowAsJavaScriptException();
//     return env.Null();
//   }

//   uint32_t start = info[0].As<Napi::Number>().Uint32Value();
//   uint32_t end   = info[1].As<Napi::Number>().Uint32Value();
//   const auto N   = boost::num_vertices(g);

//   std::cerr << "[route] FindPath called with start=" << start
//             << " end=" << end << " (N=" << N << ")\n";

//   // 2) Bounds check
//   if (start >= N || end >= N) {
//     std::string msg = "startIdx/endIdx out of range (0.." + std::to_string(N-1) + ")";
//     Napi::RangeError::New(env, msg).ThrowAsJavaScriptException();
//     return env.Null();
//   }

//   // 3) Trivial case: same node
//   if (start == end) {
//     std::cerr << "[route] start == end, returning single‐node path\n";
//     Napi::Array single = Napi::Array::New(env, 1);
//     Napi::Object obj   = Napi::Object::New(env);
//     obj.Set("nodeIdx", start);
//     obj.Set("lat",      nodeCoords[start].first);
//     obj.Set("lon",      nodeCoords[start].second);
//     single.Set((uint32_t)0, obj);
//     return single;
//   }

//   try {
//     // 4) Run Dijkstra
//     std::vector<float>    dist(N,  std::numeric_limits<float>::infinity());
//     std::vector<uint32_t> pred(N,  start);  // default predecessor
//     std::cerr << "[route] running Dijkstra…\n";
//     boost::dijkstra_shortest_paths(
//       g,
//       start,
//       boost::predecessor_map(&pred[0])
//         .distance_map(&dist[0])
//     );
//     std::cerr << "[route] Dijkstra done, dist[end]=" << dist[end] << "\n";

//     // 5) Check reachability
//     if (dist[end] == std::numeric_limits<float>::infinity()) {
//       throw std::runtime_error("No route found between start and end");
//     }

//     // 6) Reconstruct path safely
//     std::vector<uint32_t> path;
//     path.reserve(128);
//     uint32_t v = end;
//     for (size_t steps = 0; v != start; ++steps) {
//       if (steps > N) {
//         throw std::runtime_error("Cycle detected during path reconstruction");
//       }
//       path.push_back(v);
//       v = pred[v];
//     }
//     path.push_back(start);
//     std::reverse(path.begin(), path.end());
//     std::cerr << "[route] path reconstructed, length=" << path.size() << "\n";

//     // 7) Convert to JS array
//     Napi::Array jsPath = Napi::Array::New(env, path.size());
//     for (uint32_t i = 0; i < path.size(); ++i) {
//       uint32_t idx = path[i];
//       Napi::Object obj = Napi::Object::New(env);
//       obj.Set("nodeIdx", idx);
//       obj.Set("lat",      nodeCoords[idx].first);
//       obj.Set("lon",      nodeCoords[idx].second);
//       jsPath.Set(i, obj);
//     }
//     return jsPath;
//   }
//   catch (const Napi::Error& e) {
//     // Already a JS exception
//     e.ThrowAsJavaScriptException();
//     return env.Null();
//   }
//   catch (const std::exception& e) {
//     std::cerr << "[route] exception: " << e.what() << "\n";
//     Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
//     return env.Null();
//   }
//   catch (...) {
//     std::cerr << "[route] unknown error\n";
//     Napi::Error::New(env, "Unknown error in FindPath").ThrowAsJavaScriptException();
//     return env.Null();
//   }
// }

Napi::Value FindPath(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  const auto N = boost::num_vertices(g);

  // 1) Validate args
  if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "Expected (startIdx: number, endIdx: number)")
      .ThrowAsJavaScriptException();
    return env.Null();
  }
  uint32_t start = info[0].As<Napi::Number>().Uint32Value();
  uint32_t end   = info[1].As<Napi::Number>().Uint32Value();

  std::cerr << "[route] FindPath(" << start << ", " << end << ")\n";

  // 2) Bounds check
  if (start >= N || end >= N) {
    Napi::RangeError::New(
      env,
      "startIdx/endIdx out of range [0.." + std::to_string(N-1) + "]"
    ).ThrowAsJavaScriptException();
    return env.Null();
  }

  // 3) Trivial same-node case
  if (start == end) {
    Napi::Array single = Napi::Array::New(env, 1);
    Napi::Object obj   = Napi::Object::New(env);
    obj.Set("nodeIdx", start);
    obj.Set("lat",      nodeCoords[start].first);
    obj.Set("lon",      nodeCoords[start].second);
    single.Set((uint32_t)0, obj);
    return single;
  }

  try {
    // Run Dijkstra
    std::vector<float>    dist(N, std::numeric_limits<float>::infinity());
    std::vector<uint32_t> pred(N);
    for (uint32_t i = 0; i < N; ++i) pred[i] = i;

    boost::dijkstra_shortest_paths(
      g,
      start,
      boost::predecessor_map(&pred[0]).distance_map(&dist[0])
    );

    // *** NEW: detect unreachable via isinf() ***
    if (std::isinf(dist[end])) {
      // no path found
      std::cerr << "[route] no path: dist[" << end << "] is infinite\n";
      // return an empty JS array
      return Napi::Array::New(env, 0);
    }

    // Now safe to reconstruct
    std::vector<uint32_t> path;
    uint32_t v = end;
    while (true) {
      path.push_back(v);
      if (v == start) break;
      uint32_t p = pred[v];
      // should never happen now
      if (p == v) {
        std::cerr << "[route] unexpected broken chain at " << v << "\n";
        return Napi::Array::New(env, 0);
      }
      v = p;
    }
    std::reverse(path.begin(), path.end());

    // Convert to JS array as before
    Napi::Array jsPath = Napi::Array::New(env, path.size());
    for (uint32_t i = 0; i < path.size(); ++i) {
      uint32_t idx = path[i];
      Napi::Object obj = Napi::Object::New(env);
      obj.Set("nodeIdx", idx);
      obj.Set("lat",      nodeCoords[idx].first);
      obj.Set("lon",      nodeCoords[idx].second);
      jsPath.Set(i, obj);
    }
    return jsPath;
  }
  catch (const std::exception& ex) {
    std::cerr << "[route] exception: " << ex.what() << "\n";
    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  LoadGraph("../data/graph_nodes.bin", "../data/graph_edges.bin");
  exports.Set("findPath", Napi::Function::New(env, FindPath));
  return exports;
}

NODE_API_MODULE(route, Init);
