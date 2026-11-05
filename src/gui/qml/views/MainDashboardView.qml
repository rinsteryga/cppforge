import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var dashboardController

    signal moduleSelected(int moduleId)
    signal duelsSelected()
    signal profileSelected()
    signal logoutSelected()

    property int activeNav: 0 // 0: Learn, 1: Duels, 2: Profile

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // 1. LEFT SIDEBAR (Original CppForge Style)
        // ==========================================
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            color: Theme.bgSidebar

            // Right border dividing sidebar from content
            Rectangle {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: Theme.borderSubtle
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 35
                anchors.bottomMargin: 30
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 12

                // Logo Container (Dynamic with theme)
                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 100
                    source: Theme.logoPath
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                Item {
                    Layout.preferredHeight: 25
                }

                // Nav Button: LEARN
                Rectangle {
                    id: btnLearn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    radius: 12
                    color: navLearnMouse.containsMouse ? (Theme.isDark ? "#21262d" : "#f1f5f9") : "transparent"
                    border.width: root.activeNav === 0 ? 2 : 0
                    border.color: Theme.accent

                    // Bottom extra border line like original widget
                    Rectangle {
                        visible: root.activeNav === 0
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 4
                        radius: 2
                        color: Theme.accent
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        text: "LEARN"
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Black
                        font.letterSpacing: 1.2
                        color: root.activeNav === 0 ? Theme.accent : Theme.textSecondary
                    }

                    MouseArea {
                        id: navLearnMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.activeNav = 0
                    }
                }

                // Nav Button: DUELS
                Rectangle {
                    id: btnDuels
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    radius: 12
                    color: navDuelsMouse.containsMouse ? (Theme.isDark ? "#21262d" : "#f1f5f9") : "transparent"
                    border.width: root.activeNav === 1 ? 2 : 0
                    border.color: Theme.accent

                    Rectangle {
                        visible: root.activeNav === 1
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 4
                        radius: 2
                        color: Theme.accent
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        text: "DUELS"
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Black
                        font.letterSpacing: 1.2
                        color: root.activeNav === 1 ? Theme.accent : Theme.textSecondary
                    }

                    MouseArea {
                        id: navDuelsMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.activeNav = 1
                            root.duelsSelected()
                        }
                    }
                }

                // Nav Button: PROFILE
                Rectangle {
                    id: btnProfile
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    radius: 12
                    color: navProfileMouse.containsMouse ? (Theme.isDark ? "#21262d" : "#f1f5f9") : "transparent"
                    border.width: root.activeNav === 2 ? 2 : 0
                    border.color: Theme.accent

                    Rectangle {
                        visible: root.activeNav === 2
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 4
                        radius: 2
                        color: Theme.accent
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        text: "PROFILE"
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Black
                        font.letterSpacing: 1.2
                        color: root.activeNav === 2 ? Theme.accent : Theme.textSecondary
                    }

                    MouseArea {
                        id: navProfileMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.activeNav = 2
                            root.profileSelected()
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                // Nav Button: LOGOUT
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 44
                    radius: 10
                    color: logoutMouse.containsMouse ? Theme.dangerBg : "transparent"
                    border.width: 1
                    border.color: logoutMouse.containsMouse ? Theme.danger : "transparent"

                    Behavior on color { ColorAnimation { duration: Theme.animFast } }

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 8

                        Text {
                            text: "LOGOUT"
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Bold
                            font.letterSpacing: 1.0
                            color: logoutMouse.containsMouse ? Theme.danger : Theme.textMuted
                        }
                    }

                    MouseArea {
                        id: logoutMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (root.dashboardController) {
                                root.dashboardController.logout()
                            }
                            root.logoutSelected()
                        }
                    }
                }
            }
        }

        // ==========================================
        // 2. MAIN CONTENT AREA (2-Column Dashboard)
        // ==========================================
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.fill: parent
                anchors.margins: 25
                spacing: 25

                // LEFT COLUMN: Modules List (takes remaining width)
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ModulesView {
                        anchors.fill: parent
                        dashboardController: root.dashboardController
                        onModuleSelected: function(moduleId) {
                            root.moduleSelected(moduleId)
                        }
                    }
                }

                // RIGHT COLUMN: Digest & Stats
                ColumnLayout {
                    Layout.preferredWidth: 320
                    Layout.maximumWidth: 340
                    Layout.minimumWidth: 280
                    Layout.fillHeight: true
                    spacing: 20

                    // Card 1: Interesting Fact (💡)
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: factCol.implicitHeight + 40
                        radius: 12
                        color: Theme.bgCard
                        border.width: 1
                        border.color: Theme.borderSubtle

                        ColumnLayout {
                            id: factCol
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 12

                            Row {
                                spacing: 8
                                Text {
                                    text: "💡"
                                    font.pixelSize: 18
                                }
                                Text {
                                    text: "Interesting fact"
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 17
                                    font.weight: Font.Bold
                                    color: Theme.textPrimary
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                text: root.dashboardController ? root.dashboardController.tipOfTheDay : ""
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                lineHeight: 1.4
                                wrapMode: Text.WordWrap
                                color: Theme.textSecondary
                            }
                        }
                    }

                    // Card 2: Daily Goal (🎯)
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: goalCol.implicitHeight + 40
                        radius: 12
                        color: Theme.bgCard
                        border.width: 1
                        border.color: Theme.borderSubtle

                        ColumnLayout {
                            id: goalCol
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 12

                            Row {
                                spacing: 8
                                Text {
                                    text: "🎯"
                                    font.pixelSize: 18
                                }
                                Text {
                                    text: "Daily Goal"
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 17
                                    font.weight: Font.Bold
                                    color: Theme.textPrimary
                                }
                            }

                            Text {
                                text: "Solved today: " + (root.dashboardController ? root.dashboardController.dailyGoalSolved : 0) + " / " + (root.dashboardController ? root.dashboardController.dailyGoalTarget : 3)
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                color: Theme.textSecondary
                            }

                            ForgeProgressBar {
                                Layout.fillWidth: true
                                from: 0
                                to: root.dashboardController ? root.dashboardController.dailyGoalTarget : 3
                                value: root.dashboardController ? root.dashboardController.dailyGoalSolved : 0
                                barColor: (root.dashboardController && root.dashboardController.dailyGoalSolved >= root.dashboardController.dailyGoalTarget) ? Theme.success : Theme.accent
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
