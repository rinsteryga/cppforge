set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Test Widgets Gui Core)

configure_file(
    ${CMAKE_SOURCE_DIR}/.env.example
    ${CMAKE_CURRENT_BINARY_DIR}/.env.example
    COPYONLY
)

# Определяем список GUI-тестов
set(GUI_TESTS
    test_AuthWindow
    test_SignUpWindow
    test_MainWindow
    test_CustomTitleBar
)

file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/**/test_*.cpp"
)

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE})

    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/gui/include
        ${CMAKE_SOURCE_DIR}/src/core/include
        ${CMAKE_SOURCE_DIR}/src/data
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE
            cppforge_core
            cppforge_data
            cppforge_gui
            Qt5::Test
            Qt5::Widgets
            Qt5::Gui
            Qt5::Core
            Qt5::Sql
    )

    # Проверяем, является ли это GUI-тестом
    list(FIND GUI_TESTS ${TEST_NAME} IS_GUI_TEST)
    
    if(${IS_GUI_TEST} GREATER -1)
        # Для GUI-тестов используем minimal платформу для headless режима
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        set_tests_properties(${TEST_NAME} PROPERTIES
            ENVIRONMENT "QT_QPA_PLATFORM=minimal"
            TIMEOUT 60
        )
    else()
        # Обычные тесты
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 60)
    endif()
endforeach()
