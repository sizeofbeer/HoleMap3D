cmake_minimum_required(VERSION 3.11)

project(holemap)

include_directories(assets/
                    base/
                    core/
                    dataIO/
                    tools/
                    vender/)

file (GLOB HOLE_SOURCES
                    base/*.cpp
                    core/*.cpp
                    dataIO/*.cpp
                    tools/*cpp
                    vender/*cpp)

set(SOURCES ${HOLE_SOURCES})

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
enable_abcg(${PROJECT_NAME})
