import QtQuick
import QtQuick.Layouts
import CppForge

Rectangle {
    id: control

    property string iconName: ""
    property string iconText: ""
    property string text: ""
    property string badgeText: ""
    property string badgeVariant: "neutral"
    property bool showChevron: false
    property bool isActive: false

    signal clicked()

    implicitWidth: 200
    implicitHeight: 38
    radius: Theme.radiusMd

    color: control.isActive
        ? Theme.accentLight
        : (mouseArea.containsMouse ? Theme.bgCardHover : "transparent")

    border.width: 1
    border.color: control.isActive
        ? Theme.accent
        : (mouseArea.containsMouse ? Theme.borderSubtle : "transparent")

    Behavior on color { ColorAnimation { duration: Theme.animFast } }
    Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 10

        // Left accent indicator for active item
        Rectangle {
            visible: control.isActive
            Layout.preferredWidth: 3
            Layout.preferredHeight: 18
            radius: 1.5
            color: Theme.accent
        }

        // Custom Vector Icon
        ForgeIcon {
            visible: control.iconName !== ""
            name: control.iconName
            size: 16
            color: control.isActive ? Theme.accent : (mouseArea.containsMouse ? Theme.textPrimary : Theme.textSecondary)
            Layout.alignment: Qt.AlignVCenter
        }

        // Fallback text icon
        Text {
            visible: control.iconName === "" && control.iconText !== ""
            text: control.iconText
            font.pixelSize: 15
            Layout.alignment: Qt.AlignVCenter
        }

        // Title
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            text: control.text
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeMd
            font.weight: control.isActive ? Font.Bold : Font.Normal
            color: control.isActive ? Theme.accent : (mouseArea.containsMouse ? Theme.textPrimary : Theme.textSecondary)
            elide: Text.ElideRight
        }

        // Optional Badge
        ForgeBadge {
            visible: control.badgeText !== ""
            text: control.badgeText
            variant: control.badgeVariant
            Layout.alignment: Qt.AlignVCenter
        }

        // Optional Chevron
        Text {
            visible: control.showChevron
            text: "›"
            font.family: Theme.fontFamily
            font.pixelSize: 16
            font.weight: Font.Bold
            color: Theme.textMuted
            Layout.alignment: Qt.AlignVCenter
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: control.clicked()
    }
}
