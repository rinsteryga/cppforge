file(GLOB_RECURSE DATA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(cppforge_data STATIC
    ${DATA_SOURCES}
)

find_package(Qt5 REQUIRED COMPONENTS Sql)

target_include_directories(cppforge_data PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(cppforge_data
    PUBLIC 
    Qt5::Sql
    Qt5::Core
    cppforge_core
)
