cmake_minimum_required(VERSION 3.10)
project(HelBikeIngest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# ─── Paths for header‐only dependencies ───────────────────────────────────
set(OSMIUM_INCLUDE_DIR   "/Users/willdonnelly/src/libosmium/include")
set(PROTOZERO_INCLUDE_DIR "/Users/willdonnelly/src/protozero/include")

# ─── Find Boost (installed via Homebrew under /opt/homebrew) ─────────────
set(BOOST_ROOT "/opt/homebrew/Cellar/boost")
find_package(Boost REQUIRED COMPONENTS program_options filesystem)

# ─── buildGraph executable ───────────────────────────────────────────────
add_executable(buildGraph buildGraph.cpp)

target_include_directories(buildGraph PRIVATE
  ${OSMIUM_INCLUDE_DIR}
  ${PROTOZERO_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
)

# Link Boost + compression/XML libs required by Osmium
target_link_libraries(buildGraph PRIVATE
  Boost::filesystem
  Boost::program_options
  bz2          # bzip2
  expat        # Expat XML parser
  z            # zlib
)

# # ─── buildKdTree executable ───────────────────────────────────────────────
# add_executable(buildKdTree buildKdTree.cpp)

# target_include_directories(buildKdTree PRIVATE
#   ${OSMIUM_INCLUDE_DIR}
#   ${PROTOZERO_INCLUDE_DIR}
#   ${Boost_INCLUDE_DIRS}
# )

# target_link_libraries(buildKdTree PRIVATE
#   Boost::filesystem
#   Boost::program_options
#   bz2
#   expat
#   z
# )
