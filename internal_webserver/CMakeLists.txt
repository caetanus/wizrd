cmake_minimum_required(VERSION 3.1)
project(wizrd_ws)

file(GLOB WS_SRC
    "./*.cpp"
    "./*.h")

include_directories(".")


add_library(wizrd_ws SHARED
            ${WS_SRC})
