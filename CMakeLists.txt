cmake_minimum_required(VERSION 3.28.1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
project(raytracing-in-one-week)
add_executable(main main.cpp)
include_directories(/home/illusionary/package/stb)