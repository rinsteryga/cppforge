cmake_minimum_required(VERSION 3.19)
project(Qtlearning LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(Qtlearning
    main.cpp
    src/gui/src/mainwindow.cpp
    src/gui/include//mainwindow.hpp
    src/gui/src/authwindow.cpp
    src/gui/include/authwindow.hpp
    src/gui/src/signupwindow.cpp
    src/gui/include/signupwindow.hpp
)

target_link_libraries(Qtlearning Qt5::Core Qt5::Widgets)
