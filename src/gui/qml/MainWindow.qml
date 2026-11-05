import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Window {
    id: mainWin

    visible: false
    width: 1280
    height: 820
    minimumWidth: 960
    minimumHeight: 640

    title: "cppforge"
    flags: Qt.Window | Qt.FramelessWindowHint
    color: Theme.bgApp

    property var dashboardController

    signal openRoadmap(int moduleId, string title)
    signal logout()

    TaskController {
        id: taskCtrl
        Component.onCompleted: {
            if (mainWin.dashboardController) {
                taskCtrl.setUserId(mainWin.dashboardController.userId)
            }
        }
    }

    // 0: Learn Dashboard, 1: Duels, 2: Profile, 3: Roadmap, 4: Task
    property int currentPage: 0
    property int activeNav: 0 // 0: LEARN, 1: DUELS, 2: PROFILE

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Custom Frameless Title Bar with Theme Toggle & Window Controls
        ForgeTitleBar {
            id: titleBar
            Layout.fillWidth: true
            title: {
                if (currentPage === 3) return "cppforge — " + (dashboardController ? dashboardController.currentModuleTitle : "Roadmap")
                if (currentPage === 4) return "cppforge — " + taskCtrl.title
                if (activeNav === 1) return "cppforge — Multiplayer Duels"
                if (activeNav === 2) return "cppforge — Profile & Statistics"
                return "cppforge — Main Menu"
            }
            targetWindow: mainWin
        }

        // Main App Layout: Sidebar (Left) + Content (Right)
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // ==========================================
            // 1. LEFT SIDEBAR (KDE Breeze Style)
            // ==========================================
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 220
                color: Theme.bgSidebar

                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 1
                    color: Theme.borderSubtle
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.topMargin: 24
                    anchors.bottomMargin: 20
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    spacing: 8

                    // Dynamic Logo (White in dark theme, Black in light theme, transparent background)
                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 80
                        source: Theme.logoPath
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }

                    // Search Field (Discover & System Settings style)
                    ForgeSearchField {
                        Layout.fillWidth: true
                        placeholderText: "Search..."
                        onTextChanged: {
                            // Can filter modules or challenges by text
                        }
                    }

                    Item {
                        Layout.preferredHeight: 6
                    }

                    // Nav Button: LEARN
                    ForgeNavItem {
                        Layout.fillWidth: true
                        iconName: "learn"
                        text: "Learn"
                        isActive: mainWin.activeNav === 0
                        showChevron: true
                        onClicked: {
                            mainWin.activeNav = 0
                            mainWin.currentPage = 0
                        }
                    }

                    // Nav Button: DUELS
                    ForgeNavItem {
                        Layout.fillWidth: true
                        iconName: "duels"
                        text: "Duels"
                        isActive: mainWin.activeNav === 1
                        badgeText: "PvP"
                        badgeVariant: "accent"
                        showChevron: true
                        onClicked: {
                            mainWin.activeNav = 1
                            mainWin.currentPage = 1
                        }
                    }

                    // Nav Button: PROFILE
                    ForgeNavItem {
                        Layout.fillWidth: true
                        iconName: "profile"
                        text: "Profile"
                        isActive: mainWin.activeNav === 2
                        showChevron: true
                        onClicked: {
                            mainWin.activeNav = 2
                            mainWin.currentPage = 2
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    // Nav Button: LOGOUT
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        radius: Theme.radiusMd
                        color: logoutMouse.containsMouse ? Theme.dangerBg : "transparent"
                        border.width: 1
                        border.color: logoutMouse.containsMouse ? Theme.danger : "transparent"

                        Behavior on color { ColorAnimation { duration: Theme.animFast } }

                        Row {
                            anchors.left: parent.left
                            anchors.leftMargin: 14
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 10

                            ForgeIcon {
                                name: "logout"
                                size: 15
                                color: logoutMouse.containsMouse ? Theme.danger : Theme.textSecondary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: "Logout"
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeSm
                                font.weight: Font.DemiBold
                                color: logoutMouse.containsMouse ? Theme.danger : Theme.textSecondary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            id: logoutMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (mainWin.dashboardController) {
                                    mainWin.dashboardController.logout()
                                }
                                mainWin.logout()
                            }
                        }
                    }
                }
            }

            // ==========================================
            // 2. CONTENT CONTAINER (Pages Switcher)
            // ==========================================
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Page 0: Main Learning Dashboard (Modules + Digest)
                Item {
                    anchors.fill: parent
                    visible: mainWin.currentPage === 0

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 24
                        spacing: 24

                        // Modules List (stretches to fill remaining width)
                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            ModulesView {
                                anchors.fill: parent
                                dashboardController: mainWin.dashboardController
                                onModuleSelected: function(moduleId) {
                                    if (mainWin.dashboardController) {
                                        mainWin.dashboardController.loadRoadmap(moduleId)
                                    }
                                    mainWin.currentPage = 3
                                }
                            }
                        }

                        // Digest & Stats Cards (Fixed clean width matching original widget UI)
                        ColumnLayout {
                            Layout.preferredWidth: 320
                            Layout.maximumWidth: 340
                            Layout.minimumWidth: 280
                            Layout.fillHeight: true
                            spacing: 20

                            // Interesting Fact Card
                            ForgeCard {
                                Layout.fillWidth: true
                                title: "Interesting Fact"
                                iconName: "bulb"
                                iconColor: Theme.warning
                                hasAccentTop: true
                                accentColor: Theme.warning

                                Text {
                                    width: parent.width
                                    text: mainWin.dashboardController ? mainWin.dashboardController.tipOfTheDay : ""
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                    lineHeight: 1.4
                                    wrapMode: Text.WordWrap
                                    color: Theme.textSecondary
                                }
                            }

                            // Daily Goal Card (System Monitor Radial Gauge)
                            ForgeCard {
                                Layout.fillWidth: true
                                title: "Daily Goal"
                                iconName: "target"
                                iconColor: Theme.accent
                                hasAccentTop: true
                                accentColor: Theme.accent

                                ForgeRadialGauge {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    implicitWidth: 150
                                    implicitHeight: 140
                                    title: ""
                                    minValue: 0
                                    maxValue: mainWin.dashboardController ? mainWin.dashboardController.dailyGoalTarget : 3
                                    value: mainWin.dashboardController ? mainWin.dashboardController.dailyGoalSolved : 0
                                    valueText: (mainWin.dashboardController ? mainWin.dashboardController.dailyGoalSolved : 0) + " / " + (mainWin.dashboardController ? mainWin.dashboardController.dailyGoalTarget : 3)
                                    detailText: "Tasks completed"
                                    gaugeColor: (mainWin.dashboardController && mainWin.dashboardController.dailyGoalSolved >= mainWin.dashboardController.dailyGoalTarget) ? Theme.success : Theme.accent
                                }
                            }

                            Item { Layout.fillHeight: true }
                        }
                    }
                }

                // Page 1: Duels
                DuelView {
                    anchors.fill: parent
                    visible: mainWin.currentPage === 1
                    dashboardController: mainWin.dashboardController
                }

                // Page 2: Profile & Statistics
                ProfileView {
                    anchors.fill: parent
                    visible: mainWin.currentPage === 2
                    dashboardController: mainWin.dashboardController
                }

                // Page 3: Roadmap (Lessons Tree)
                RoadmapView {
                    anchors.fill: parent
                    visible: mainWin.currentPage === 3
                    dashboardController: mainWin.dashboardController
                    onBackRequested: mainWin.currentPage = 0
                    onLessonSelected: function(lessonId) {
                        taskCtrl.loadTask(lessonId)
                        mainWin.currentPage = 4
                    }
                }

                // Page 4: Task / Code Editor
                TaskView {
                    anchors.fill: parent
                    visible: mainWin.currentPage === 4
                    taskController: taskCtrl
                    onBackRequested: mainWin.currentPage = 3
                }
            }
        }
    }
}
