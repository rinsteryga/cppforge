import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property alias text: input.text
    property alias placeholderText: input.placeholderText
    property alias echoMode: input.echoMode
    property alias readOnly: input.readOnly
    property alias maximumLength: input.maximumLength
    property alias validator: input.validator
    property alias inputFocus: input.focus

    property string label: ""
    property string errorMessage: ""
    property bool isPassword: false
    property bool showPassword: false
    property string leadingIcon: ""

    signal accepted()

    implicitWidth: 320
    implicitHeight: columnLayout.implicitHeight

    Column {
        id: columnLayout
        width: parent.width
        spacing: Theme.spacingXs

        // Optional field label
        Text {
            id: labelText
            visible: root.label !== ""
            text: root.label
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeSm
            font.weight: Font.Medium
            color: root.errorMessage !== "" ? Theme.danger : Theme.textSecondary
        }

        // Input container box (KDE Breeze line edit)
        Rectangle {
            id: box
            width: parent.width
            height: 36
            radius: Theme.radiusMd
            color: input.activeFocus ? Theme.bgInputHover : Theme.bgInput
            border.width: 1
            border.color: {
                if (root.errorMessage !== "") return Theme.danger
                if (input.activeFocus) return Theme.borderFocus
                return Theme.borderStrong
            }

            Behavior on border.color {
                ColorAnimation { duration: Theme.animFast }
            }
            Behavior on color {
                ColorAnimation { duration: Theme.animFast }
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMd
                anchors.rightMargin: Theme.spacingSm
                spacing: Theme.spacingSm

                // Leading icon if provided
                Image {
                    id: leadIcon
                    visible: root.leadingIcon !== ""
                    source: root.leadingIcon
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                    Layout.alignment: Qt.AlignVCenter
                    fillMode: Image.PreserveAspectFit
                }

                TextField {
                    id: input
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeMd
                    color: Theme.textPrimary
                    placeholderTextColor: Theme.textMuted
                    selectionColor: Theme.accentLight
                    selectedTextColor: Theme.accent

                    verticalAlignment: TextInput.AlignVCenter
                    echoMode: root.isPassword ? (root.showPassword ? TextInput.Normal : TextInput.Password) : TextInput.Normal

                    background: null
                    padding: 0
                    leftPadding: 0
                    rightPadding: 0

                    onAccepted: root.accepted()
                }

                // Password toggle button
                Item {
                    id: toggleEyeBtn
                    visible: root.isPassword
                    Layout.preferredWidth: 28
                    Layout.preferredHeight: 28
                    Layout.alignment: Qt.AlignVCenter

                    Image {
                        anchors.centerIn: parent
                        width: 16
                        height: 16
                        source: root.showPassword ? "qrc:/images/eye_open.png" : "qrc:/images/eye_slash.png"
                        fillMode: Image.PreserveAspectFit
                        opacity: eyeMouse.containsMouse ? 1.0 : 0.6
                        Behavior on opacity { NumberAnimation { duration: Theme.animFast } }
                    }

                    MouseArea {
                        id: eyeMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.showPassword = !root.showPassword
                    }
                }
            }
        }

        // Error message label below
        Text {
            id: errText
            visible: root.errorMessage !== ""
            text: root.errorMessage
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeXs
            color: Theme.danger
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }
}
