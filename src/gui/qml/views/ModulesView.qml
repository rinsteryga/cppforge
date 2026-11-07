import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var dashboardController
    signal moduleSelected(int moduleId)

    ListView {
        id: modulesList
        anchors.fill: parent
        spacing: 14
        clip: true

        model: dashboardController ? dashboardController.modules : []

        delegate: Rectangle {
            id: card
            width: ListView.view.width
            height: 98
            radius: Theme.radiusLg
            color: modelData.isLocked ? (Theme.isDark ? "#26292c" : "#f5f6f7") : (cardMouse.containsMouse ? Theme.bgCardHover : Theme.bgCard)
            border.width: 1
            border.color: cardMouse.containsMouse && !modelData.isLocked ? Theme.accent : Theme.borderStrong

            Behavior on color { ColorAnimation { duration: Theme.animFast } }
            Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

            MouseArea {
                id: cardMouse
                anchors.fill: parent
                hoverEnabled: !modelData.isLocked
                cursorShape: modelData.isLocked ? Qt.ArrowCursor : Qt.PointingHandCursor
                onClicked: {
                    if (!modelData.isLocked && root.dashboardController) {
                        root.moduleSelected(modelData.id)
                    }
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        Layout.fillWidth: true
                        text: modelData.title
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeLg
                        font.weight: Font.Bold
                        color: modelData.isLocked ? Theme.textMuted : Theme.textPrimary
                        elide: Text.ElideRight
                    }

                    // Start Learning / Locked Button (KDE Breeze style)
                    Rectangle {
                        Layout.preferredWidth: 130
                        Layout.preferredHeight: 32
                        radius: Theme.radiusMd
                        color: modelData.isLocked ? (Theme.isDark ? "#2a2e32" : "#e0e2e4") : (btnMouse.containsMouse ? Theme.accentHover : Theme.accent)
                        border.width: 1
                        border.color: modelData.isLocked ? Theme.borderSubtle : (btnMouse.containsMouse ? Theme.accentHover : Theme.accentPressed)

                        Behavior on color { ColorAnimation { duration: Theme.animFast } }

                        Row {
                            anchors.centerIn: parent
                            spacing: 6

                            ForgeLockIcon {
                                size: 13
                                color: Theme.textMuted
                                holeColor: Theme.isDark ? "#2a2e32" : "#e0e2e4"
                                visible: modelData.isLocked
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: modelData.isLocked ? "Locked" : (modelData.progress === 100 ? "✓ Review" : (modelData.progress > 0 ? "Continue" : "Start Learning"))
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                font.weight: Font.Bold
                                color: modelData.isLocked ? Theme.textMuted : Theme.textOnAccent
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            id: btnMouse
                            anchors.fill: parent
                            enabled: !modelData.isLocked
                            hoverEnabled: true
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onClicked: {
                                if (!modelData.isLocked && root.dashboardController) {
                                    root.moduleSelected(modelData.id)
                                }
                            }
                        }
                    }
                }

                // Progress Bar & Percentage
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    ForgeProgressBar {
                        Layout.fillWidth: true
                        value: modelData.progress
                        barColor: modelData.progress === 100 ? Theme.success : Theme.accent
                    }

                    Text {
                        text: modelData.progress + "% completed"
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: Theme.textSecondary
                    }
                }
            }
        }
    }
}
