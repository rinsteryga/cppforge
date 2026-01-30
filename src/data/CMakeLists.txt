add_library(cppforge_data STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories/PgUserRepository.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories/PgUserRepository.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DataBaseConnection.cpp
)

target_link_libraries(cppforge_data
    PRIVATE 
    Qt5::Core
)