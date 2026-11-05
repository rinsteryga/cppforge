import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var dashboardController
    signal backRequested()
    signal lessonSelected(int lessonId)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        // 1. Navigation & Header bar
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            ForgeButton {
                text: "← Back to Modules"
                variant: "secondary"
                onClicked: root.backRequested()
            }

            Text {
                text: root.dashboardController ? root.dashboardController.currentModuleTitle : "Module Roadmap"
                font.family: Theme.fontFamily
                font.pixelSize: 20
                font.weight: Font.Bold
                color: Theme.textPrimary
            }

            Item { Layout.fillWidth: true }
        }

        // 2. Interactive Roadmap Path (KDE Breeze Card)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.radiusLg
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderStrong
            clip: true

            Flickable {
                id: flick
                anchors.fill: parent
                contentWidth: width
                contentHeight: pathCol.implicitHeight + 80
                clip: true

                Column {
                    id: pathCol
                    width: Math.min(parent.width - 40, 600)
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 40
                    spacing: 0

                    Repeater {
                        model: root.dashboardController ? root.dashboardController.roadmapNodes : []

                        delegate: Item {
                            width: pathCol.width
                            height: 90

                            // Connecting vertical line to next node
                            Rectangle {
                                visible: index < (root.dashboardController ? root.dashboardController.roadmapNodes.length - 1 : 0)
                                anchors.horizontalCenter: nodeCircle.horizontalCenter
                                anchors.top: nodeCircle.bottom
                                width: 2
                                height: 48
                                color: modelData.isCompleted ? Theme.accent : Theme.borderStrong
                            }

                            // Interactive Lesson Card (KDE Breeze style)
                            Rectangle {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 64
                                radius: Theme.radiusMd
                                color: modelData.isLocked ? (Theme.isDark ? "#26292c" : "#f5f6f7") : (cardMouse.containsMouse ? Theme.bgCardHover : Theme.bgApp)
                                border.width: 1
                                border.color: cardMouse.containsMouse && !modelData.isLocked ? Theme.accent : Theme.borderStrong

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 16
                                    anchors.rightMargin: 16
                                    spacing: 16

                                    // Status Circle
                                    Rectangle {
                                        id: nodeCircle
                                        Layout.preferredWidth: 42
                                        Layout.preferredHeight: 42
                                        radius: 21
                                        color: modelData.isCompleted ? Theme.success : (modelData.isLocked ? (Theme.isDark ? "#21262d" : "#e2e8f0") : Theme.accent)

                                        Text {
                                            anchors.centerIn: parent
                                            text: modelData.isCompleted ? "✓" : (modelData.isLocked ? "🔒" : "▶")
                                            font.pixelSize: 15
                                            font.weight: Font.Bold
                                            color: modelData.isLocked ? Theme.textMuted : "white"
                                        }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Text {
                                            text: "Lesson " + modelData.index
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 12
                                            font.weight: Font.DemiBold
                                            color: Theme.textSecondary
                                        }

                                        Text {
                                            text: modelData.title
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 15
                                            font.weight: Font.Bold
                                            color: modelData.isLocked ? Theme.textMuted : Theme.textPrimary
                                        }
                                    }

                                    // Action status badge
                                    Rectangle {
                                        Layout.preferredHeight: 30
                                        Layout.preferredWidth: badgeText.implicitWidth + 16
                                        radius: 6
                                        color: modelData.isCompleted ? Theme.successBg : (modelData.isLocked ? "transparent" : Theme.accentLight)

                                        Text {
                                            id: badgeText
                                            anchors.centerIn: parent
                                            text: modelData.isCompleted ? "Completed" : (modelData.isLocked ? "Locked" : "Start Task")
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 11
                                            font.weight: Font.Bold
                                            color: modelData.isCompleted ? Theme.success : (modelData.isLocked ? Theme.textMuted : Theme.accent)
                                        }
                                    }
                                }

                                MouseArea {
                                    id: cardMouse
                                    anchors.fill: parent
                                    enabled: !modelData.isLocked
                                    hoverEnabled: true
                                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    onClicked: {
                                        if (!modelData.isLocked) {
                                            root.lessonSelected(modelData.id)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
