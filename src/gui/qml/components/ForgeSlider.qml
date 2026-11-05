import QtQuick
import QtQuick.Controls.Basic
import CppForge

Slider {
    id: control

    from: 0.0
    to: 100.0
    value: 50.0

    implicitWidth: 200
    implicitHeight: 28

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: control.availableWidth
        height: implicitHeight
        radius: 2
        color: Theme.borderSubtle

        // Filled active track
        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            color: control.enabled ? Theme.accent : Theme.borderStrong
            radius: 2
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 18
        implicitHeight: 18
        radius: 9
        color: control.pressed ? Theme.accentPressed : (control.hovered ? Theme.bgCardHover : Theme.bgCard)
        border.color: control.hovered || control.pressed ? Theme.accent : Theme.borderStrong
        border.width: 1.5

        Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
        Behavior on color { ColorAnimation { duration: Theme.animFast } }
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
