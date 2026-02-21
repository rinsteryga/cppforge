set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

add_library(cppforge_gui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AuthWindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SignUpWindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CustomTitleBar.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SignUpWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CustomTitleBar.cpp

    resources/resources.qrc
)

target_include_directories(cppforge_gui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cppforge_gui
    PRIVATE 
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Sql
    cppforge_core
    cppforge_data
)
