file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(cppforge_app STATIC ${APP_SOURCES} ${APP_HEADERS})

target_include_directories(cppforge_app PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cppforge_app PUBLIC cppforge_core Qt6::Core)
