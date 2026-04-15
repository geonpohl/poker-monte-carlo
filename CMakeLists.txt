cmake_minimum_required(VERSION 3.20)

project(poker_monte_carlo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(poker_core src/simulator.cpp)
target_include_directories(poker_core PUBLIC include)

add_executable(poker_cli apps/equity_cli.cpp)
target_link_libraries(poker_cli PRIVATE poker_core)
