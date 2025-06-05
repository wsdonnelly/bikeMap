#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/node.hpp>

#include <cmath>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>

// 1a) Handler for collecting bike-friendly ways
struct WayCollector : public osmium::handler::Handler
{
    std::unordered_map<uint64_t, std::vector<uint64_t>>& wayNodesMap;

    WayCollector(std::unordered_map<uint64_t, std::vector<uint64_t>>& wayNodesMap) :
        wayNodesMap(wayNodesMap) {}

    void way(const osmium::Way& way)
    {
        auto& tags = way.tags();
        const char* hv = tags.get_value_by_key("highway");
        const char* bv = tags.get_value_by_key("bicycle");

        if ((hv  != nullptr && std::strcmp(hv, "cycleway") == 0) ||
            (bv  != nullptr && std::strcmp(bv, "yes")      == 0))
        {
            // std::cout << "added: " << way.id() << ": ";
            wayNodesMap[way.id()] = std::vector<uint64_t>();
            for (auto& n : way.nodes())
            {
                wayNodesMap[way.id()].push_back(n.ref());
            }
            // std::cout << wayNodesMap[way.id()].size() <<std::endl;
        }
    }
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: buildGraph <path-to-osm-pbf>\n";
        return 1;
    }
    const char* osmFile = argv[1];

    std::unordered_map<uint64_t, std::vector<uint64_t>> wayNodesMap;
    WayCollector wc(wayNodesMap);

    // ─── Pass 1: Collect bike-friendly ways ──────────────────────────────
    osmium::io::Reader reader(osmFile);
    osmium::apply(reader, wc);
    reader.close();

    std::cout << "waynodesmap size: " << wayNodesMap.size() << "\n";

    // Build neededNodeIds
    std::unordered_set<uint64_t> neededNodeIds;

    for (auto& [wayId, nodesList] : wayNodesMap) {
        for (uint64_t nid : nodesList) {
            neededNodeIds.insert(nid);
        }
    }
    std::cout << "Will collect coords for " << neededNodeIds.size() << " nodes.\n";

    return 0;
}