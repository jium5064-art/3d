cmake_minimum_required(VERSION 3.20)
project(Physics3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib 5.0 REQUIRED)

add_executable(Physics3D main.cpp)
target_link_libraries(Physics3D PRIVATE raylib)
