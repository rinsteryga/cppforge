find_package(Qt5 REQUIRED COMPONENTS Test)

configure_file(
    ${CMAKE_SOURCE_DIR}/.env.example
    ${CMAKE_CURRENT_BINARY_DIR}/.env.example
    COPYONLY
)

file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/**/test_*.cpp"
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME}
        PRIVATE
            cppforge_core
            Qt5::Test
    )

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
