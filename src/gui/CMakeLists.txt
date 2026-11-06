set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Sql Network)

file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE GUI_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(cppforge_gui STATIC
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

target_include_directories(cppforge_gui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cppforge_gui
    PRIVATE 
    Qt6::Widgets 
    Qt6::Core 
    Qt6::Sql
    Qt6::Network
    cppforge_core
    cppforge_data
)
