find_package(Qt6 REQUIRED COMPONENTS Core Sql Concurrent Network)
find_package(asio CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)

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
)

target_compile_definitions(cppforge_server PRIVATE ASIO_STANDALONE)

target_link_libraries(cppforge_server PRIVATE
    Crow::Crow
    cppforge_core
    cppforge_data
    Qt6::Core
    Qt6::Sql
    Qt6::Concurrent
    Qt6::Network
)

if(WIN32)
    target_link_libraries(cppforge_server PRIVATE ws2_32 mswsock)
endif()
