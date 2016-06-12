cmake_minimum_required(VERSION 2.6)
project(wizrd_internal_ws)

file(GLOB WS_SRC
    "./*.cpp"
    "./*.h")

include_directories(".")

add_library(wizrd_ws STATIC
            ${WS_SRC})
