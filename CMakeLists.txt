cmake_minimum_required(VERSION 3.20)
project(untitled)

find_package(SFML REQUIRED COMPONENTS graphics window system)

add_executable(untitled main.cpp)

target_link_libraries(untitled
        sfml-graphics sfml-window sfml-system
)
