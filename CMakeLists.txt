cmake_minimum_required(VERSION 3.28)
project(GraphDraw LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# --- SFML ---
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# --- muParser ---
FetchContent_Declare(muparser
    GIT_REPOSITORY https://github.com/beltoforion/muparser.git
    GIT_TAG v2.3.5
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(muparser)

add_executable(main src/main.cpp
    src/GraphDraw.cpp
)

target_include_directories(main PRIVATE src)

target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics muparser)
