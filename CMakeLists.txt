cmake_minimum_required(VERSION 3.3)
project(ray)
add_definitions(-std=c++11 -Wall -Werror -march=native -fno-exceptions)

add_subdirectory(src)
