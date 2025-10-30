enable_testing()

find_package(Qt5 REQUIRED COMPONENTS Test)

add_executable(TestCore
    unit/test_auth.cpp
    unit/test_user.cpp
)

target_link_libraries(TestCore
    PRIVATE
    CoreModule
    Qt5::Test
)

add_test(NAME CoreTests COMMAND TestCore)

add_executable(TestGui
    gui/test_mainwindow.cpp
)

target_link_libraries(TestGui
    PRIVATE
    GuiModule
    CoreModule
    Qt5::Test
    Qt5::Widgets
)

add_test(NAME GuiTests COMMAND TestGui)