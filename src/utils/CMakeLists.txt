cmake_minimum_required(VERSION 2.6)
project(utilities)

file(GLOB WS_SRC
     "./*.cpp"
     "./*.h")

include_directories(".")
include_directories("../src")

add_library(wizrd_util STATIC
            ${WS_SRC})
