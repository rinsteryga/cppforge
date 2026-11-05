import QtQuick
import QtQuick.Layouts
import CppForge

Rectangle {
    id: control

    property string title: ""
    property string description: ""
    property string iconSource: ""
    property string iconName: "package"
    property string buttonText: "Open"
    property string buttonVariant: "secondary"
    property string badgeText: ""
    property string badgeVariant: "neutral"
    property bool buttonVisible: true

    signal actionClicked()
    signal cardClicked()

    implicitWidth: 320
    implicitHeight: 76
    radius: Theme.radiusLg

    color: mouseArea.containsMouse ? Theme.bgCardHover : Theme.bgCard
    border.width: 1
    border.color: mouseArea.containsMouse ? Theme.borderStrong : Theme.borderSubtle

    Behavior on color { ColorAnimation { duration: Theme.animFast } }
    Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 14
        anchors.rightMargin: 14
        spacing: 14

        // Icon Box (like KDE Discover app icon)
        Rectangle {
            Layout.preferredWidth: 46
            Layout.preferredHeight: 46
            radius: Theme.radiusMd
            color: Theme.bgInput
            border.width: 1
            border.color: Theme.borderSubtle

            Image {
                anchors.fill: parent
                anchors.margins: 6
                source: control.iconSource
                visible: control.iconSource !== ""
                fillMode: Image.PreserveAspectFit
                smooth: true
            }

            ForgeIcon {
                anchors.centerIn: parent
                visible: control.iconSource === ""
                name: control.iconName
                size: 22
                color: Theme.accent
            }
        }

        // Title and Description Column
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 2

            RowLayout {
                spacing: 8
                Text {
                    text: control.title
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Font.Bold
                    color: Theme.textPrimary
                    elide: Text.ElideRight
                }

                ForgeBadge {
                    visible: control.badgeText !== ""
                    text: control.badgeText
                    variant: control.badgeVariant
                }
            }

            Text {
                Layout.fillWidth: true
                text: control.description
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSm
                color: Theme.textSecondary
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }

        // Action Button
        ForgeButton {
            visible: control.buttonVisible
            text: control.buttonText
            variant: control.buttonVariant
            implicitHeight: 30
            implicitWidth: 78
            onClicked: control.actionClicked()
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        z: -1
        onClicked: control.cardClicked()
    }
}
