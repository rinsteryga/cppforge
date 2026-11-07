find_package(Qt6 REQUIRED COMPONENTS Test Sql Network)

configure_file(
    ${CMAKE_SOURCE_DIR}/.env.example
    ${CMAKE_CURRENT_BINARY_DIR}/.env.example
    COPYONLY
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/**/test_*.cpp"
)

add_custom_target(cppforge_tests)

foreach(TEST_FILE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)

    add_executable(${TEST_NAME} "${TEST_FILE}")
    target_link_libraries(${TEST_NAME}
        PRIVATE
            cppforge_core
            cppforge_data
            cppforge_api
            cppforge_app
            Qt6::Test
            Qt6::Sql
            Qt6::Network
    )

    if(TEST_NAME STREQUAL "test_ServerIntegration")
        target_link_libraries(${TEST_NAME} PRIVATE cppforge_server_lib)
    else()
        target_link_libraries(${TEST_NAME} PRIVATE cppforge_qpsql_plugin)
    endif()

    if(WIN32 AND (TEST_NAME STREQUAL "test_ServerIntegration" OR TEST_NAME STREQUAL "test_DataBaseConnection" OR
                  TEST_NAME STREQUAL "test_SqlDriverAvailability") AND
       DEFINED QT6_INSTALL_PREFIX AND DEFINED QT6_INSTALL_PLUGINS)
        set(QT_SQL_DRIVER_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/sqldrivers")
        if(EXISTS "${QT_SQL_DRIVER_DIR}")
            add_custom_command(TARGET ${TEST_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory
                        "${QT_SQL_DRIVER_DIR}" "$<TARGET_FILE_DIR:${TEST_NAME}>/sqldrivers"
                COMMENT "Deploying the Qt SQL driver for ${TEST_NAME}"
                VERBATIM
            )
        endif()

        foreach(_runtime_dll libpq.dll libssl-3-x64.dll libcrypto-3-x64.dll)
            if(EXISTS "${QT6_INSTALL_PREFIX}/bin/${_runtime_dll}")
                add_custom_command(TARGET ${TEST_NAME} POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                            "${QT6_INSTALL_PREFIX}/bin/${_runtime_dll}"
                            "$<TARGET_FILE_DIR:${TEST_NAME}>/${_runtime_dll}"
                    COMMENT "Deploying ${_runtime_dll} for ${TEST_NAME}"
                    VERBATIM
                )
            endif()
        endforeach()
    endif()

    target_compile_definitions(${TEST_NAME} PRIVATE CPPFORGE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    add_dependencies(cppforge_tests ${TEST_NAME})
endforeach()
