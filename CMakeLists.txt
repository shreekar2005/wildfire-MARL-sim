cmake_minimum_required(VERSION 3.10)
project(wildfireMARLsim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src/environment environment)
add_subdirectory(src/agent agent)
add_subdirectory(src/simulation simulation)
add_subdirectory(src/marl marl)

add_executable(wildfire_marl_sim src/main.cpp)

target_link_libraries(wildfire_marl_sim PRIVATE
    environment_lib
    agent_lib
    simulation_lib
    marl_train_lib
    marl_test_lib
    raylib
    Threads::Threads
    redis++
)