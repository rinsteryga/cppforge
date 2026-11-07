find_package(Qt6 REQUIRED COMPONENTS Core Sql Network)
find_package(asio CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)

add_library(cppforge_server_lib STATIC
    src/ServerApp.cpp
    src/ServerConfig.cpp
    src/CorsMiddleware.cpp
    src/RequestLoggingMiddleware.cpp
    src/controllers/HealthController.cpp
    src/controllers/ModulesController.cpp
)

# Qt6 exposes the PostgreSQL driver plugin as Qt6::QPSQLDriverPlugin. A static Qt build
# (the vcpkg x64-linux default) needs both the plugin library and Q_IMPORT_PLUGIN in
# every final executable; a shared build loads the plugin at runtime instead.
add_library(cppforge_qpsql_plugin INTERFACE)
if(TARGET Qt6::QPSQLDriverPlugin)
    get_target_property(_cppforge_qpsql_plugin_type Qt6::QPSQLDriverPlugin TYPE)
    if(_cppforge_qpsql_plugin_type STREQUAL "STATIC_LIBRARY")
        target_sources(cppforge_qpsql_plugin INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/QpsqlStaticPlugin.cpp")
        target_compile_definitions(cppforge_qpsql_plugin INTERFACE CPPFORGE_STATIC_QPSQL)
        target_link_libraries(cppforge_qpsql_plugin INTERFACE Qt6::QPSQLDriverPlugin)
    endif()
endif()

target_include_directories(cppforge_server_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(cppforge_server_lib PUBLIC ASIO_STANDALONE)

target_link_libraries(cppforge_server_lib
    PUBLIC cppforge_api cppforge_app cppforge_data Crow::Crow Qt6::Core Qt6::Sql Qt6::Network
           cppforge_qpsql_plugin
)

add_executable(cppforge_server src/main.cpp)
target_compile_definitions(cppforge_server PRIVATE ASIO_STANDALONE)
target_link_libraries(cppforge_server PRIVATE cppforge_server_lib)

if(WIN32)
    target_compile_definitions(cppforge_server_lib PUBLIC _WIN32_WINNT=0x0A00)
    target_link_libraries(cppforge_server_lib PUBLIC ws2_32 mswsock)

    if(DEFINED QT6_INSTALL_PREFIX AND DEFINED QT6_INSTALL_PLUGINS)
        set(QT_SQL_DRIVER_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/sqldrivers")
        if(EXISTS "${QT_SQL_DRIVER_DIR}")
            add_custom_command(TARGET cppforge_server POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory
                        "${QT_SQL_DRIVER_DIR}" "$<TARGET_FILE_DIR:cppforge_server>/sqldrivers"
                COMMENT "Deploying the Qt SQL driver for cppforge_server"
                VERBATIM
            )
        endif()

        foreach(_runtime_dll libpq.dll libssl-3-x64.dll libcrypto-3-x64.dll)
            if(EXISTS "${QT6_INSTALL_PREFIX}/bin/${_runtime_dll}")
                add_custom_command(TARGET cppforge_server POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                            "${QT6_INSTALL_PREFIX}/bin/${_runtime_dll}"
                            "$<TARGET_FILE_DIR:cppforge_server>/${_runtime_dll}"
                    COMMENT "Deploying ${_runtime_dll} for cppforge_server"
                    VERBATIM
                )
            endif()
        endforeach()
    endif()
endif()
