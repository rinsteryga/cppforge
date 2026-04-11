cmake_minimum_required(VERSION 3.19)
project(cppforge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CPPFORGE_ENABLE_SANITIZERS "Enable Address and Undefined Behavior Sanitizers" OFF)
if(CPPFORGE_ENABLE_SANITIZERS AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU"))
    add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

add_executable(cppforge main.cpp)

target_link_libraries(cppforge
    PRIVATE 
    cppforge_core
    cppforge_gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Core
)

set_target_properties(cppforge PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE ON
)

install(TARGETS cppforge DESTINATION bin)
install(FILES "src/data/migrations/schema.sql" DESTINATION "data/migrations")
install(FILES "src/data/migrations/seed.sql" DESTINATION "data/migrations")
install(FILES "scripts/install_deps.ps1" DESTINATION "scripts")

# Deploy Qt libraries
install(CODE "
    execute_process(
        COMMAND C:/msys64/ucrt64/bin/windeployqt.exe --no-translations --no-opengl-sw \${CMAKE_INSTALL_PREFIX}/bin/cppforge.exe
    )
")

# Deploy PostgreSQL driver dependencies from MSYS2
install(FILES 
    "C:/msys64/ucrt64/bin/libpq.dll"
    "C:/msys64/ucrt64/bin/libintl-8.dll"
    "C:/msys64/ucrt64/bin/libiconv-2.dll"
    "C:/msys64/ucrt64/bin/libssl-3-x64.dll"
    "C:/msys64/ucrt64/bin/libcrypto-3-x64.dll"
    DESTINATION bin
)

set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "cppforge")
set(CPACK_PACKAGE_VENDOR "cppforge")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    ExecWait 'powershell.exe -ExecutionPolicy Bypass -File \\\"$INSTDIR/scripts/install_deps.ps1\\\" -InstallDir \\\"$INSTDIR\\\"'
")

include(CPack)
