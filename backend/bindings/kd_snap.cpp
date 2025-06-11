#include <napi.h>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg  = boost::geometry;
namespace bgi = boost::geometry::index;

// Point type in degrees
using Point = bg::model::point<double,2,bg::cs::geographic<bg::degree>>;
// (Point, treeIndex)
using Value = std::pair<Point, uint32_t>;

// In-memory storage
static bgi::rtree<Value, bgi::quadratic<16>> tree;
static std::vector<float> kdLat, kdLon;

// LoadKD: must be called before any other function
void LoadKD(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    throw std::runtime_error("Could not open KD file: " + path);
  }
  uint32_t N;
  in.read(reinterpret_cast<char*>(&N), sizeof(N));
  if (!in) {
    throw std::runtime_error("Failed to read KD header");
  }

  kdLat.resize(N);
  kdLon.resize(N);
  std::vector<Value> data;
  data.reserve(N);

  for (uint32_t i = 0; i < N; ++i) {
    float lat, lon;
    uint32_t idx;              // this is our tree index
    in.read(reinterpret_cast<char*>(&lat), sizeof(lat));
    in.read(reinterpret_cast<char*>(&lon), sizeof(lon));
    in.read(reinterpret_cast<char*>(&idx), sizeof(idx));
    if (!in) {
      throw std::runtime_error("Unexpected EOF in KD data");
    }
    kdLat[i] = lat;
    kdLon[i] = lon;
    data.emplace_back(Point{lat, lon}, idx);
  }

  // Build the R-tree for nearest-neighbor
  tree = bgi::rtree<Value, bgi::quadratic<16>>(data.begin(), data.end());
}

// findNearest(lat, lon) → treeIndex
Napi::Value FindNearest(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "Expected (number, number)").ThrowAsJavaScriptException();
    return env.Null();
  }
  double lat = info[0].As<Napi::Number>().DoubleValue();
  double lon = info[1].As<Napi::Number>().DoubleValue();

  Point q(lat, lon);
  std::vector<Value> result;
  tree.query(bgi::nearest(q, 1), std::back_inserter(result));
  uint32_t treeIdx = result.front().second;
  return Napi::Number::New(env, treeIdx);
}

// getNode(treeIndex) → { nodeIdx, lat, lon }
Napi::Value GetNode(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Expected (number)").ThrowAsJavaScriptException();
    return env.Null();
  }
  uint32_t idx = info[0].As<Napi::Number>().Uint32Value();
  if (idx >= kdLat.size()) {
    Napi::RangeError::New(env, "Index out of range").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Object obj = Napi::Object::New(env);
  obj.Set("nodeIdx", idx);
  obj.Set("lat",      Napi::Number::New(env, kdLat[idx]));
  obj.Set("lon",      Napi::Number::New(env, kdLon[idx]));
  return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  try {
    LoadKD("../data/kd_nodes.bin");
  } catch (const std::exception& e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return exports;
  }

  exports.Set("findNearest", Napi::Function::New(env, FindNearest));
  exports.Set("getNode",      Napi::Function::New(env, GetNode));
  return exports;
}

NODE_API_MODULE(kd_snap, Init);
