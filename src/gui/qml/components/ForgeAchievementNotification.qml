import QtQuick
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property string achievementTitle: ""
    property string achievementDescription: ""
    property string achievementIcon: "qrc:/images/lb.png"

    implicitWidth: 380
    implicitHeight: 88

    visible: opacity > 0
    opacity: 0
    y: -height - 20

    function notify(title, description, iconPath) {
        achievementTitle = title
        achievementDescription = description
        if (iconPath && iconPath.length > 0) {
            if (iconPath.startsWith(":/")) {
                achievementIcon = "qrc" + iconPath
            } else if (!iconPath.startsWith("qrc:")) {
                achievementIcon = "qrc:/" + iconPath
            } else {
                achievementIcon = iconPath
            }
        } else {
            achievementIcon = "qrc:/images/lb.png"
        }

        animShow.restart()
    }

    Rectangle {
        id: card
        anchors.fill: parent
        radius: Theme.radiusLg
        color: Theme.isDark ? "#2a2e32" : "#ffffff"
        border.width: 2
        border.color: "#f39c12" // Gold accent

        layer.enabled: true

        RowLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 14

            // Achievement Icon
            Rectangle {
                Layout.preferredWidth: 54
                Layout.preferredHeight: 54
                radius: Theme.radiusMd
                color: Theme.isDark ? "#1b1e20" : "#f8fafc"
                border.width: 1
                border.color: "#f39c12"

                Image {
                    anchors.centerIn: parent
                    width: 44
                    height: 44
                    source: root.achievementIcon
                    fillMode: Image.PreserveAspectFit
                }
            }

            // Text Info
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 3

                RowLayout {
                    spacing: 6
                    Text {
                        text: "🏆 ACHIEVEMENT UNLOCKED"
                        font.family: Theme.fontFamily
                        font.pixelSize: 10
                        font.weight: Font.Bold
                        color: "#f39c12"
                    }
                }

                Text {
                    text: root.achievementTitle
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.Bold
                    color: Theme.textPrimary
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: root.achievementDescription
                    font.family: Theme.fontFamily
                    font.pixelSize: 11
                    color: Theme.textSecondary
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }
    }

    SequentialAnimation {
        id: animShow

        ParallelAnimation {
            NumberAnimation {
                target: root
                property: "y"
                from: -root.height - 20
                to: 24
                duration: 400
                easing.type: Easing.OutBack
            }
            NumberAnimation {
                target: root
                property: "opacity"
                from: 0
                to: 1
                duration: 350
            }
        }

        PauseAnimation {
            duration: 4000
        }

        ParallelAnimation {
            NumberAnimation {
                target: root
                property: "y"
                from: 24
                to: -root.height - 20
                duration: 350
                easing.type: Easing.InQuad
            }
            NumberAnimation {
                target: root
                property: "opacity"
                from: 1
                to: 0
                duration: 300
            }
        }
    }
}
