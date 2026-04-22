cmake_minimum_required(VERSION 3.20)

# Give the project a name and tell CMake we are building C++ code.
project(poker_monte_carlo LANGUAGES CXX)

# Use a modern C++ language version, but keep compiler-specific extensions off.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

# Build our reusable code as a library target.
# Right now the library is tiny, but later it will hold most of the poker logic.
add_library(
    poker_core
    src/card.cpp
    src/deck.cpp
    src/hand.cpp
    src/simulator.cpp
)

# Any target that links to poker_core can include headers from include/.
target_include_directories(poker_core PUBLIC include)

# Build the command-line program.
add_executable(poker_cli apps/equity_cli.cpp)
add_executable(poker_perf apps/perf_benchmark.cpp)

# Link the CLI against our library so main() can use poker_core code.
target_link_libraries(poker_cli PRIVATE poker_core)
target_link_libraries(poker_perf PRIVATE poker_core)

if(BUILD_TESTING)
    add_executable(poker_hand_tests tests/hand_tests.cpp)
    target_link_libraries(poker_hand_tests PRIVATE poker_core)
    add_test(NAME poker_hand_tests COMMAND poker_hand_tests)
endif()
