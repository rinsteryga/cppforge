# src/data/CMakeLists.txt

set(CPPFORGE_DATA_SOURCES
    DataBaseConnection.cpp
    repositories/PgUserRepository.cpp
)

add_library(cppforge_data STATIC
    ${CPPFORGE_DATA_SOURCES}
)

target_include_directories(cppforge_data
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories
)

target_link_libraries(cppforge_data
    PRIVATE 
    Qt5::Core      
    Qt5::Sql       
    PUBLIC
    cppforge_core
)