add_library(cppforge_gui STATIC
    include/mainwindow.hpp
    include/authwindow.hpp
    include/signupwindow.hpp
    
    src/mainwindow.cpp
    src/authwindow.cpp
    src/signupwindow.cpp
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