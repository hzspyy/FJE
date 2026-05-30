cmake_minimum_required(VERSION 3.16)

project(FJE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_subdirectory(external/json)
add_subdirectory(external/argparse)

add_executable(FJE ${SOURCES})
target_link_libraries(FJE nlohmann_json::nlohmann_json argparse)

target_include_directories(FJE PRIVATE include)
