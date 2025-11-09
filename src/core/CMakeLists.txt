add_library(cppforge_core STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/User.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/Achievement.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/Module.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/Lesson.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/CodingTask.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/TestCase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/ExecutionResult.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/UserProgress.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities/Submission.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/repositories/IUserRepository.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/User.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Achievement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Module.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Lesson.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/CodingTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/TestCase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/ExecutionResult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/UserProgress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Submission.cpp
)

target_include_directories(cppforge_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cppforge_core
    PRIVATE 
    Qt5::Core
)