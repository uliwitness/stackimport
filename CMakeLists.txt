cmake_minimum_required(VERSION 3.22)
project(stackimport)

set(CMAKE_CXX_STANDARD 23)

add_executable(stackimport main.cpp Picture.cpp Picture.hpp WOBA.cpp WOBA.hpp)
