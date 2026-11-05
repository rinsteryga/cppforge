set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Sql Network Quick Qml QuickControls2)

file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE GUI_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(cppforge_gui STATIC
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    resources/resources.qrc
)

set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(cppforge_gui
    URI CppForge
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        qml/Theme.qml
        qml/AuthWindow.qml
        qml/MainWindow.qml
        qml/components/ForgeButton.qml
        qml/components/ForgeTextField.qml
        qml/components/ForgeCard.qml
        qml/components/ForgeTitleBar.qml
        qml/components/ForgeProgressBar.qml
        qml/components/ForgeIcon.qml
        qml/components/ForgeSearchField.qml
        qml/components/ForgeNavItem.qml
        qml/components/ForgeBadge.qml
        qml/components/ForgeRadialGauge.qml
        qml/components/ForgeActionCard.qml
        qml/components/ForgeSwitch.qml
        qml/components/ForgeSegmentedControl.qml
        qml/components/ForgeThemeCard.qml
        qml/components/ForgeSlider.qml
        qml/components/ForgeStatCard.qml
        qml/components/AuthBackgroundDecorations.qml
        qml/views/AuthView.qml
        qml/views/SignUpView.qml
        qml/views/ModulesView.qml
        qml/views/MainDashboardView.qml
        qml/views/ProfileView.qml
        qml/views/RoadmapView.qml
        qml/views/TaskView.qml
        qml/views/DuelView.qml
)

target_include_directories(cppforge_gui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cppforge_gui
    PUBLIC
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
    PRIVATE 
    Qt6::Widgets 
    Qt6::Sql
    Qt6::Network
    cppforge_core
    cppforge_data
)
