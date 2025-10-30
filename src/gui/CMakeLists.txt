add_library(cppforge_gui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AuthWindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SignUpWindow.hpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SignUpWindow.cpp
)

target_include_directories(cppforge_gui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cppforge_gui
    PRIVATE 
    Qt5::Widgets
    Qt5::Core
    cppforge_core
)