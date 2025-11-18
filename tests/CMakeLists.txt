find_package(Qt5 REQUIRED COMPONENTS Test)

add_executable(TestCore
    unit/test_PasswordHashGenerator.cpp
)

target_link_libraries(TestCore
    PRIVATE
    cppforge_core
    Qt5::Test
)

add_test(NAME CoreTests COMMAND TestCore)
