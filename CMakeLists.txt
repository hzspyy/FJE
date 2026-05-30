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

enable_testing()

set(FJE_TEST_JSON ${CMAKE_CURRENT_SOURCE_DIR}/test/test.json)
set(FJE_ICON_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/icon_config.json)

add_test(NAME fje_help COMMAND FJE --help)
add_test(NAME fje_list_icons COMMAND FJE --list-icons -c ${FJE_ICON_CONFIG})
add_test(NAME fje_tree
         COMMAND FJE -f ${FJE_TEST_JSON} -c ${FJE_ICON_CONFIG})
add_test(NAME fje_rectangle
         COMMAND FJE -f ${FJE_TEST_JSON} -s rectangle -i pokerface -c
                 ${FJE_ICON_CONFIG})
add_test(NAME fje_no_icons
         COMMAND FJE -f ${FJE_TEST_JSON} --no-icons)
add_test(NAME fje_invalid_icon
         COMMAND FJE -f ${FJE_TEST_JSON} -i missing -c ${FJE_ICON_CONFIG})

set_tests_properties(fje_invalid_icon PROPERTIES WILL_FAIL TRUE)
