find_package(Qt5 REQUIRED COMPONENTS Test Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

configure_file(
    ${CMAKE_SOURCE_DIR}/.env.example
    ${CMAKE_CURRENT_BINARY_DIR}/.env.example
    COPYONLY
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/**/test_*.cpp"
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE})

    # Determine which libraries to link based on test type
    if(TEST_NAME MATCHES "^test_(MainWindow|AuthWindow|SignUpWindow|TaskWindow|CustomTitleBar|TaskManager)$")
        # GUI tests need GUI libraries
        target_link_libraries(${TEST_NAME}
            PRIVATE
                cppforge_core
                cppforge_data
                cppforge_gui
                Qt5::Test
                Qt5::Gui
                Qt5::Widgets
                Qt5::Sql
        )
        # Run GUI tests with offscreen platform
        add_test(NAME ${TEST_NAME} 
            COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=offscreen 
            ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}
        )
    else()
        # Non-GUI tests
        target_link_libraries(${TEST_NAME}
            PRIVATE
                cppforge_core
                cppforge_data
                Qt5::Test
                Qt5::Sql
        )
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
endforeach()
