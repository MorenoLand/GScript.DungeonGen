cmake_minimum_required(VERSION 3.14)
project(dungeongen CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dungeongen
    src/Main.cpp
    src/Utils.cpp
    src/Options.cpp
    src/Layout.cpp
    src/DungeonMaker.cpp
    src/Level.cpp
    src/GMap.cpp
    src/DungeonGenerator.cpp
)

if (MSVC)
    target_compile_options(dungeongen PRIVATE /W4 /utf-8)
else()
    target_compile_options(dungeongen PRIVATE -Wall -Wextra)
endif()
