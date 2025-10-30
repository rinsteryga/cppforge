add_library(cppforge_core STATIC
    include/entities/User.hpp
    include/entities/Achievement.hpp
    include/entities/Module.hpp
    
    src/entities/User.cpp
    src/entities/Achievement.cpp
    src/entities/Module.cpp
)

target_include_directories(cppforge_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cppforge_core
    PRIVATE 
    Qt5::Core
)