import QtQuick
import QtQuick.Window
import CppForge

Rectangle {
    id: root

    property string title: "CppForge"
    property Window targetWindow: Window.window
    property bool showControls: true
    property bool canMaximize: true

    height: 34
    color: Theme.bgSidebar
    border.width: 0

    // Bottom subtle border separating title bar from window content
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: Theme.divider
    }

    // Drag area for window movement
    MouseArea {
        id: dragRegion
        anchors.fill: parent
        anchors.rightMargin: root.showControls ? windowButtonsRow.width : 0

        onPressed: {
            if (mouse.button === Qt.LeftButton && root.targetWindow) {
                root.targetWindow.startSystemMove()
            }
        }

        onDoubleClicked: {
            if (mouse.button === Qt.LeftButton && root.targetWindow && root.canMaximize) {
                if (root.targetWindow.visibility === Window.Maximized) {
                    root.targetWindow.showNormal()
                } else {
                    root.targetWindow.showMaximized()
                }
            }
        }
    }

    // Logo & title
    Row {
        anchors.left: parent.left
        anchors.leftMargin: Theme.spacingMd
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.spacingSm

        Image {
            id: appLogo
            source: Theme.logoPath
            width: 18
            height: 18
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectFit
        }

        Text {
            text: root.title
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeSm
            font.weight: Font.Medium
            color: Theme.textPrimary
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // Window controls (minimize, maximize, close)
    Row {
        id: windowButtonsRow
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        visible: root.showControls

        // Theme Toggle Button (Sun / Moon)
        Rectangle {
            width: 38
            height: parent.height
            color: themeMouse.containsMouse ? (Theme.isDark ? "#3a3f44" : "#e0e2e4") : "transparent"

            Text {
                anchors.centerIn: parent
                text: Theme.isDark ? "☀️" : "🌙"
                font.pixelSize: 13
                color: Theme.textSecondary
            }

            MouseArea {
                id: themeMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: Theme.toggleTheme()
            }
        }

        // Minimize Button
        Rectangle {
            width: 40
            height: parent.height
            color: minMouse.containsMouse ? (Theme.isDark ? "#3a3f44" : "#e0e2e4") : "transparent"

            Rectangle {
                width: 10
                height: 1
                color: Theme.textSecondary
                anchors.centerIn: parent
            }

            MouseArea {
                id: minMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if (root.targetWindow) root.targetWindow.showMinimized()
                }
            }
        }

        // Maximize / Restore Button
        Rectangle {
            visible: root.canMaximize
            width: 40
            height: parent.height
            color: maxMouse.containsMouse ? (Theme.isDark ? "#3a3f44" : "#e0e2e4") : "transparent"

            Rectangle {
                width: 9
                height: 9
                color: "transparent"
                border.width: 1
                border.color: Theme.textSecondary
                anchors.centerIn: parent
            }

            MouseArea {
                id: maxMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if (!root.targetWindow) return
                    if (root.targetWindow.visibility === Window.Maximized) {
                        root.targetWindow.showNormal()
                    } else {
                        root.targetWindow.showMaximized()
                    }
                }
            }
        }

        // Close Button (KDE Breeze Red hover)
        Rectangle {
            width: 42
            height: parent.height
            color: closeMouse.containsMouse ? Theme.danger : "transparent"

            Canvas {
                anchors.centerIn: parent
                width: 10
                height: 10
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    ctx.strokeStyle = closeMouse.containsMouse ? "#ffffff" : (Theme.isDark ? "#8b949e" : "#656d76")
                    ctx.lineWidth = 1.2
                    ctx.beginPath()
                    ctx.moveTo(1, 1)
                    ctx.lineTo(9, 9)
                    ctx.moveTo(9, 1)
                    ctx.lineTo(1, 9)
                    ctx.stroke()
                }

                Connections {
                    target: closeMouse
                    function onContainsMouseChanged() { parent.requestPaint() }
                }
            }

            MouseArea {
                id: closeMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if (root.targetWindow) root.targetWindow.close()
                }
            }
        }
    }
}
