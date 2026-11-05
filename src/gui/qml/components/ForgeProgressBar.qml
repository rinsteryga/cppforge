import QtQuick
import CppForge

Item {
    id: root

    property real from: 0
    property real to: 100
    property real value: 0
    property color barColor: Theme.accent
    property color bgColor: Theme.isDark ? "#1b1e20" : "#e0e2e4"

    implicitHeight: 8
    implicitWidth: 200

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: 3
        color: root.bgColor
        border.width: 1
        border.color: Theme.borderSubtle

        Rectangle {
            id: fill
            height: parent.height
            radius: 3
            color: root.barColor
            width: {
                var range = Math.max(root.to - root.from, 1.0)
                var clamped = Math.max(root.from, Math.min(root.value, root.to))
                return Math.max(0, ((clamped - root.from) / range) * bg.width)
            }

            Behavior on width {
                NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
            }
        }
    }
}
