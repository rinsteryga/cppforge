import QtQuick
import CppForge

Item {
    id: control

    property bool checked: false
    property string text: ""
    signal toggled(bool isChecked)

    implicitWidth: switchRow.implicitWidth
    implicitHeight: 24

    Row {
        id: switchRow
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter

        // The Switch Track + Thumb
        Rectangle {
            id: track
            width: 42
            height: 22
            radius: 11
            anchors.verticalCenter: parent.verticalCenter
            color: control.checked ? Theme.accent : (mouseArea.containsMouse ? Theme.bgCardHover : Theme.bgInput)
            border.width: 1
            border.color: control.checked ? Theme.accent : (mouseArea.containsMouse ? Theme.borderStrong : Theme.borderSubtle)

            Behavior on color { ColorAnimation { duration: Theme.animFast } }
            Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

            // Thumb
            Rectangle {
                id: thumb
                width: 16
                height: 16
                radius: 8
                anchors.verticalCenter: parent.verticalCenter
                x: control.checked ? (track.width - width - 3) : 3
                color: control.checked ? "white" : Theme.textSecondary

                Behavior on x {
                    NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutQuad }
                }
                Behavior on color { ColorAnimation { duration: Theme.animFast } }
            }
        }

        // Optional Label Text
        Text {
            visible: control.text !== ""
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeMd
            color: mouseArea.containsMouse ? Theme.textPrimary : Theme.textSecondary
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            control.checked = !control.checked
            control.toggled(control.checked)
        }
    }
}
