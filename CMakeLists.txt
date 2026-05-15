cmake_minimum_required(VERSION 3.19)
project(cppforge VERSION 1.0.2 LANGUAGES CXX)

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

if(WIN32)
    set(APP_ICON_RESOURCE "src/gui/resources/windows_icon.rc")
else()
    set(APP_ICON_RESOURCE "")
endif()

add_executable(cppforge main.cpp ${APP_ICON_RESOURCE})

target_link_libraries(cppforge
    PRIVATE 
    cppforge_core
    cppforge_data
    cppforge_gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Core
)

set_target_properties(cppforge PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.cppforge.app"
    MACOSX_BUNDLE_BUNDLE_NAME "cppforge"
)

install(TARGETS cppforge
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

if(WIN32)
    set(DATA_DESTINATION "data/migrations")
    set(SCRIPT_DESTINATION "scripts")
else()
    set(DATA_DESTINATION "share/cppforge/data/migrations")
    set(SCRIPT_DESTINATION "share/cppforge/scripts")
endif()

install(FILES "src/data/migrations/schema.sql" DESTINATION "${DATA_DESTINATION}")
install(FILES "src/data/migrations/seed.sql" DESTINATION "${DATA_DESTINATION}")
install(DIRECTORY "src/data/migrations/modules" DESTINATION "${DATA_DESTINATION}")
if(WIN32)
    set(PG_INSTALLER_NAME "postgresql-16.4-1-windows-x64.exe")
    set(PG_INSTALLER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${PG_INSTALLER_NAME}")
    set(PG_INSTALLER_URL "https://get.enterprisedb.com/postgresql/${PG_INSTALLER_NAME}")

    if(NOT EXISTS "${PG_INSTALLER_PATH}")
        message(STATUS "PostgreSQL installer not found in root. Downloading...")
        file(DOWNLOAD "${PG_INSTALLER_URL}" "${PG_INSTALLER_PATH}" SHOW_PROGRESS)
    else()
        message(STATUS "Found existing PostgreSQL installer: ${PG_INSTALLER_PATH}")
    endif()

    set(VCREDIST_NAME "vc_redist.x64.exe")
    set(VCREDIST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${VCREDIST_NAME}")
    set(VCREDIST_URL "https://aka.ms/vs/17/release/vc_redist.x64.exe")

    if(NOT EXISTS "${VCREDIST_PATH}")
        message(STATUS "VC++ Redistributable not found in root. Downloading...")
        file(DOWNLOAD "${VCREDIST_URL}" "${VCREDIST_PATH}" SHOW_PROGRESS)
    else()
        message(STATUS "Found existing VC++ Redistributable: ${VCREDIST_PATH}")
    endif()

    install(FILES "${PG_INSTALLER_PATH}" "${VCREDIST_PATH}" DESTINATION "scripts")


    install(FILES "scripts/install_deps.ps1" DESTINATION "scripts")
    
    get_filename_component(MSYS2_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt-qt5 windeployqt-qt5.exe windeployqt windeployqt.exe HINTS ${MSYS2_BIN_DIR})

    if(WINDEPLOYQT_EXECUTABLE)
        install(CODE "
            execute_process(
                COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" 
                        --no-translations 
                        --no-opengl-sw 
                        --no-angle 
                        --compiler-runtime 
                        \"\${CMAKE_INSTALL_PREFIX}/bin/cppforge.exe\"
                RESULT_VARIABLE WINDEPLOYQT_RES
            )
            if(NOT WINDEPLOYQT_RES EQUAL 0)
                message(FATAL_ERROR \"windeployqt failed with error: \${WINDEPLOYQT_RES}\")
            endif()
        ")
    else()
        message(WARNING "windeployqt not found! Visual assets might not load correctly on other machines.")
    endif()

    file(GLOB MSYS2_RUNTIME_DLLS
        "${MSYS2_BIN_DIR}/libpq.dll"
        "${MSYS2_BIN_DIR}/libintl-*.dll"
        "${MSYS2_BIN_DIR}/libiconv-*.dll"
        "${MSYS2_BIN_DIR}/libssl-*-x64.dll"
        "${MSYS2_BIN_DIR}/libcrypto-*-x64.dll"
        "${MSYS2_BIN_DIR}/libicuuc*.dll"
        "${MSYS2_BIN_DIR}/libicuin*.dll"
        "${MSYS2_BIN_DIR}/libicudt*.dll"
        "${MSYS2_BIN_DIR}/libdouble-conversion.dll"
        "${MSYS2_BIN_DIR}/libpcre2-16-*.dll"
        "${MSYS2_BIN_DIR}/libpcre2-8-*.dll"
        "${MSYS2_BIN_DIR}/libfreetype-*.dll"
        "${MSYS2_BIN_DIR}/libgraphite2.dll"
        "${MSYS2_BIN_DIR}/libglib-2.0-*.dll"
        "${MSYS2_BIN_DIR}/libzstd.dll"
        "${MSYS2_BIN_DIR}/libbrotlidec.dll"
        "${MSYS2_BIN_DIR}/libbrotlicommon.dll"
        "${MSYS2_BIN_DIR}/libbz2-*.dll"
        "${MSYS2_BIN_DIR}/libpng*.dll"
        "${MSYS2_BIN_DIR}/libjpeg*.dll"
        "${MSYS2_BIN_DIR}/libwebp*.dll"
        "${MSYS2_BIN_DIR}/libmd4c.dll"
        "${MSYS2_BIN_DIR}/libharfbuzz-*.dll"
        "${MSYS2_BIN_DIR}/zlib*.dll"

        "${MSYS2_BIN_DIR}/libgmp-*.dll"
        "${MSYS2_BIN_DIR}/libmpfr-*.dll"
        "${MSYS2_BIN_DIR}/libmpc-*.dll"
        "${MSYS2_BIN_DIR}/libisl-*.dll"
        "${MSYS2_BIN_DIR}/libwinpthread-*.dll"
        "${MSYS2_BIN_DIR}/libgcc_s_seh-*.dll"
        "${MSYS2_BIN_DIR}/libstdc++-*.dll"
    )

    file(GLOB GCC_BINARIES
        "${MSYS2_BIN_DIR}/gcc.exe"
        "${MSYS2_BIN_DIR}/g++.exe"
        "${MSYS2_BIN_DIR}/cpp.exe"
        "${MSYS2_BIN_DIR}/c++.exe"
        "${MSYS2_BIN_DIR}/ld.exe"
        "${MSYS2_BIN_DIR}/as.exe"
        "${MSYS2_BIN_DIR}/ar.exe"
        "${MSYS2_BIN_DIR}/strip.exe"
        "${MSYS2_BIN_DIR}/objdump.exe"
        "${MSYS2_BIN_DIR}/mingw32-make.exe"
    )

    install(FILES ${MSYS2_RUNTIME_DLLS} ${GCC_BINARIES} DESTINATION bin)

    get_filename_component(MSYS2_ROOT_DIR "${MSYS2_BIN_DIR}" DIRECTORY)

    install(DIRECTORY "${MSYS2_ROOT_DIR}/lib/gcc/" DESTINATION "lib/gcc")

    install(DIRECTORY "${MSYS2_ROOT_DIR}/include/" DESTINATION "include"
        PATTERN "Qt*" EXCLUDE
        PATTERN "boost" EXCLUDE
        PATTERN "opencv*" EXCLUDE
    )

    install(DIRECTORY "${MSYS2_ROOT_DIR}/lib/" DESTINATION "lib"
        FILES_MATCHING
        PATTERN "*.o"
        PATTERN "*.a"
        PATTERN "*.def"
        PATTERN "gcc" EXCLUDE     
        PATTERN "cmake" EXCLUDE
        PATTERN "pkgconfig" EXCLUDE
        PATTERN "libQt5*.a" EXCLUDE 
        PATTERN "libboost_*.a" EXCLUDE
        PATTERN "libopencv_*.a" EXCLUDE
    )

    install(DIRECTORY "${MSYS2_ROOT_DIR}/x86_64-w64-mingw32/" DESTINATION "x86_64-w64-mingw32")
elseif(APPLE)
    get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${QT_BIN_DIR}")

    if(NOT MACDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "macdeployqt not found!")
    endif()

    install(CODE "
        execute_process(
            COMMAND \"${MACDEPLOYQT_EXECUTABLE}\" \"\${CMAKE_INSTALL_PREFIX}/cppforge.app\"
            RESULT_VARIABLE MACDEPLOYQT_RES
        )
    ")
else()
    install(FILES "scripts/install_deps.sh" DESTINATION "${SCRIPT_DESTINATION}" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
endif()

set(CPACK_PACKAGE_NAME "cppforge")
set(CPACK_PACKAGE_VENDOR "cppforge")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/resources/icons/main_logo.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/resources/icons/main_logo.ico")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        ExecWait 'powershell.exe -ExecutionPolicy Bypass -File \\\"\$INSTDIR\\\\scripts\\\\install_deps.ps1\\\" -InstallDir \\\"\$INSTDIR\\\"'
    ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Delete \\\"\$INSTDIR\\\\.env\\\"
        Delete \\\"\$INSTDIR\\\\install_deps.log\\\"
    ")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "cppforge")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5, libqt5sql5, libqt5sql5-psql, libqt5network5, libqt5core5a, postgresql")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/scripts/postinst")
endif()

include(CPack)