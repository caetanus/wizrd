cmake_minimum_required (VERSION 2.6)
project(wizrd)
set (wizrd_VERSION_MAJOR 0)
set (wizrd_VERSION_MINOR 1)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
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
find_package(Boost 1.6.0 REQUIRED)
find_package(Boost 1.6.0 COMPONENTS log REQUIRED)

if(Boost_FOUND)
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
    add_definitions(-DBOOST_LOG_DYN_LINK)
    include_directories(${Boost_INCLUDE_DIRS})
endif()



enable_testing()
find_package(GTest REQUIRED)
# find_package(hayai)

# include_directories(${GTEST_BOTH_LIBRARIES})

add_subdirectory(internal_webserver)
add_subdirectory(Testing)


aux_source_directory(src/ SRC_LIST)
#FILE(GLOB SRC_LIST
#    src/*.h
#    src/*.cpp)

set (CMAKE_CXX_STANDARD 14)


add_executable(${PROJECT_NAME} ${SRC_LIST})
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion)


add_subdirectory(src/utils)
