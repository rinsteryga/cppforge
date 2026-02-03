# src/data/CMakeLists.txt

# Находим schema.sql
set(SCHEMA_SQL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/migrations/schema.sql")

if(EXISTS "${SCHEMA_SQL_PATH}")
    message(STATUS "Found schema.sql at: ${SCHEMA_SQL_PATH}")
    # Копируем в build директорию
    configure_file(
        "${SCHEMA_SQL_PATH}"
        "${CMAKE_CURRENT_BINARY_DIR}/schema.sql"
        COPYONLY
    )
else()
    message(WARNING "schema.sql not found at: ${SCHEMA_SQL_PATH}")
    # Создаем пустой файл чтобы избежать ошибок
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/schema.sql" "-- Empty schema\n")
endif()

# Список исходных файлов
set(CPPFORGE_DATA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories/PgUserRepository.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories/PgUserRepository.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DataBaseConnection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories/DataBaseConnection.hpp
)

add_library(cppforge_data STATIC
    ${CPPFORGE_DATA_SOURCES}
)

# Проверяем существование файлов
foreach(file ${CPPFORGE_DATA_SOURCES})
    if(NOT EXISTS ${file})
        message(WARNING "File not found: ${file}")
    endif()
endforeach()

target_include_directories(cppforge_data
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # для schema.sql
    $<INSTALL_INTERFACE:include>
)

# Связываем cppforge_data с cppforge_core
target_link_libraries(cppforge_data
    PRIVATE 
    Qt5::Core
    Qt5::Sql
    PUBLIC
    cppforge_core
)