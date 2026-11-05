import QtQuick
import QtQuick.Controls.Basic
import CppForge

Item {
    id: control

    property alias text: input.text
    property string placeholderText: "Search..."
    property int preferredHeight: 34

    signal searchAccepted(string query)
    signal cleared()

    implicitWidth: 220
    implicitHeight: preferredHeight

    Rectangle {
        id: bgRect
        anchors.fill: parent
        radius: Theme.radiusMd
        color: input.activeFocus ? Theme.bgInput : (mouseArea.containsMouse ? Theme.bgInputHover : Theme.bgInput)
        border.width: 1
        border.color: input.activeFocus ? Theme.borderFocus : (mouseArea.containsMouse ? Theme.borderStrong : Theme.borderSubtle)

        Behavior on border.color { ColorAnimation { duration: Theme.animFast } }
        Behavior on color { ColorAnimation { duration: Theme.animFast } }

        Row {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 8

            // Custom Vector Search Icon
            ForgeIcon {
                id: searchIcon
                anchors.verticalCenter: parent.verticalCenter
                name: "search"
                size: 14
                color: input.activeFocus ? Theme.accent : Theme.textMuted
            }

            // Text input field
            TextInput {
                id: input
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - searchIcon.width - (clearBtn.visible ? clearBtn.width : 0) - 20
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeMd
                color: Theme.textPrimary
                selectionColor: Theme.accent
                selectedTextColor: Theme.textOnAccent
                clip: true

                Text {
                    anchors.fill: parent
                    anchors.verticalCenter: parent.verticalCenter
                    text: control.placeholderText
                    font: input.font
                    color: Theme.textMuted
                    visible: !input.text && !input.activeFocus
                }

                onAccepted: {
                    control.searchAccepted(input.text)
                }
            }

            // Clear button (✕)
            Item {
                id: clearBtn
                anchors.verticalCenter: parent.verticalCenter
                width: 18
                height: 18
                visible: input.text.length > 0

                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: clearMouse.containsMouse ? Theme.bgCardHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        font.pixelSize: 10
                        font.family: Theme.fontFamily
                        color: clearMouse.containsMouse ? Theme.textPrimary : Theme.textMuted
                    }
                }

                MouseArea {
                    id: clearMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        input.text = ""
                        control.cleared()
                        input.forceActiveFocus()
                    }
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.IBeamCursor
        }
    }
}
