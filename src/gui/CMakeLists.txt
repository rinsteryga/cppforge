add_library(cppforge_gui STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mainwindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/authwindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/signupwindow.hpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/authwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/signupwindow.cpp
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