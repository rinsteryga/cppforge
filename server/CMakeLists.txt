find_package(Qt5 REQUIRED COMPONENTS Core Sql Concurrent Network)

include(FetchContent)

# Fetch standalone ASIO header-only library
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)

# Inform Crow where Asio include headers live
set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "Path to ASIO headers" FORCE)
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# Fetch Crow C++ Microframework from GitHub
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE SERVER_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(cppforge_server
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

target_include_directories(cppforge_server PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${asio_SOURCE_DIR}/asio/include"
)

target_compile_definitions(cppforge_server PRIVATE ASIO_STANDALONE)

target_link_libraries(cppforge_server PRIVATE
    Crow::Crow
    cppforge_core
    cppforge_data
    Qt5::Core
    Qt5::Sql
    Qt5::Concurrent
    Qt5::Network
)

if(WIN32)
    target_link_libraries(cppforge_server PRIVATE ws2_32 mswsock)
endif()
