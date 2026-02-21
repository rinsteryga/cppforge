cmake_minimum_required(VERSION 3.19)
project(cppforge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

add_executable(cppforge main.cpp)

target_link_libraries(cppforge
    PRIVATE 
    cppforge_core
    cppforge_gui
    Qt5::Widgets
    Qt5::Core
)

set_target_properties(cppforge PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
