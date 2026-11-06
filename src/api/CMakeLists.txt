find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

file(GLOB_RECURSE API_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE API_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(cppforge_api STATIC ${API_SOURCES} ${API_HEADERS})

target_include_directories(cppforge_api PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cppforge_api
    PUBLIC cppforge_core nlohmann_json::nlohmann_json
    PRIVATE Qt6::Core
)
