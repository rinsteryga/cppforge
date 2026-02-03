cmake_minimum_required(VERSION 3.19)
project(cppforge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Найти ВСЕ нужные компоненты Qt
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

add_executable(cppforge main.cpp)

# Включаем директории
target_include_directories(cppforge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data
)

# Связываем ВСЕ библиотеки
target_link_libraries(cppforge
    PRIVATE 
    cppforge_core
    cppforge_gui
    cppforge_data  # Добавляем библиотеку данных
    Qt5::Widgets
    Qt5::Core
    Qt5::Sql       # Добавляем Sql
)

set_target_properties(cppforge PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)