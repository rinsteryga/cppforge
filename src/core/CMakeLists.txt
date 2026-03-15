find_package(Qt5 REQUIRED COMPONENTS Core Sql Concurrent)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(cppforge_core STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)


target_include_directories(cppforge_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cppforge_core
    PRIVATE 
    Qt5::Core
    Qt5::Sql
    Qt5::Concurrent
)