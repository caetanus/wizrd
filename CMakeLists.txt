cmake_minimum_required (VERSION 3.1)
project(wizrd)
message(. ${CMAKE_MODULE_PATH})
set (wizrd_VERSION_MAJOR 0)
set (wizrd_VERSION_MINOR 1)

list(APPEND CMAKE_MODULE_PATH  ./cmake)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SRC "${PROJECT_SOURCE_DIR}")
set(PROJECT_CACHE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.config_cache")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/wizrd_config.h.in"
    "${PROJECT_CACHE_DIR}/wizrd_config.h"
    )

set(QMAKE_CXXFLAGS ${QMAKE_CXXFLAGS} -std=c++14)

# options section

option(USE_FCGI
    "Use fastCGI" ON)

option(USE_INTERNAL_SERVER
    "Use Internal Server" ON)

option(USE_LEGACY_CGI
    "Use Legacy CGI" OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_CACHE_DIR}")
LINK_DIRECTORIES("lib/")

# find boost
find_package(Boost 1.6.0 REQUIRED)
find_package(Boost 1.6.0 REQUIRED COMPONENTS log system thread regex)

if(Boost_FOUND)
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
    add_definitions(-DBOOST_LOG_DYN_LINK)
    include_directories(${Boost_INCLUDE_DIRS})
endif()



enable_testing()
find_package(GTest REQUIRED)
find_package(Benchmark)
# find_package(hayai)


add_subdirectory(internal_webserver)
add_subdirectory(tests)



aux_source_directory(src/ SRC_LIST)

set (CMAKE_CXX_STANDARD 11)
set (CXX_STANDARD_REQUIRED 11)
set (CXX_STANDARD 11)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME})


add_subdirectory(src/utils)
