cmake_minimum_required (VERSION 2.6)
project(wizrd)
set (wizrd_VERSION_MAJOR 0)
set (wizrd_VERSION_MINOR 1)

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SRC "${PROJECT_SOURCE_DIR}")
set(PROJECT_CACHE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.config_cache")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/wizrd_config.h.in"
    "${PROJECT_CACHE_DIR}/wizrd_config.h"
    )

# options section

option(USE_FCGI
    "Use fastCGI" ON)

option(USE_INTERNAL_SERVER
    "Use Internal Server" ON)

option(USE_LEGACY_CGI
    "Use Legacy CGI" OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_CACHE_DIR}")

# find boost
find_package(Boost 1.55.0 REQUIRED)

if(Boost_FOUND)
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
    include_directories(${Boost_INCLUDE_DIRS})
endif()

enable_testing()
find_package(GTest REQUIRED)
include_directories(wizrd ${GTEST_BOTH_LIBRARIES})


file(GLOB WIZRD_SOURCES
    "src/*.cpp"
    "src/*.h"
)
add_executable(wizrd ${WIZRD_SOURCES})
target_compile_options(wizrd PUBLIC -std=c++14 -Wall -WFloat-conversion)
