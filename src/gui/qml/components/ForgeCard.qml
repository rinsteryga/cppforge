import QtQuick
import CppForge

Rectangle {
    id: root

    property string title: ""
    property string subtitle: ""
    property string iconName: ""
    property color iconColor: Theme.accent
    property string badgeText: ""
    property string badgeVariant: "neutral"
    property bool hasAccentTop: false
    property color accentColor: Theme.accent
    property bool hoverable: false
    property bool clickable: false
    property int contentPadding: 16

    signal clicked()

    default property alias content: bodyCol.data

    radius: Theme.radiusLg
    color: (hoverable && mouseArea.containsMouse) ? Theme.bgCardHover : Theme.bgCard
    border.width: 1
    border.color: (hoverable && mouseArea.containsMouse) ? Theme.borderFocus : Theme.borderSubtle

    Behavior on color { ColorAnimation { duration: Theme.animFast } }
    Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

    implicitWidth: 320
    implicitHeight: mainCol.implicitHeight

    Column {
        id: mainCol
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        // Optional Top Accent Strip
        Rectangle {
            visible: root.hasAccentTop
            width: parent.width
            height: 3
            radius: Theme.radiusXs
            color: root.accentColor
        }

        // Optional Card Header
        Item {
            visible: root.title !== ""
            width: parent.width
            height: 44

            Row {
                anchors.fill: parent
                anchors.leftMargin: root.contentPadding
                anchors.rightMargin: root.contentPadding
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                // Icon Box
                Rectangle {
                    visible: root.iconName !== ""
                    width: 26
                    height: 26
                    radius: Theme.radiusMd
                    color: Theme.accentLight
                    anchors.verticalCenter: parent.verticalCenter

                    ForgeIcon {
                        anchors.centerIn: parent
                        name: root.iconName
                        color: root.iconColor
                        size: 13
                    }
                }

                // Titles
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 1

                    Text {
                        text: root.title
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeLg
                        font.weight: Font.Bold
                        color: Theme.textPrimary
                    }

                    Text {
                        visible: root.subtitle !== ""
                        text: root.subtitle
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeXs
                        color: Theme.textSecondary
                    }
                }

                Item {
                    width: 1
                    height: 1
                }
            }

            // Divider under header
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Theme.divider
            }
        }

        // Body Content Container
        Item {
            id: bodyContainer
            width: parent.width
            height: bodyCol.implicitHeight + (root.contentPadding * 2)

            Column {
                id: bodyCol
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: root.contentPadding
                spacing: 12
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.hoverable || root.clickable
        hoverEnabled: root.hoverable
        cursorShape: root.clickable ? Qt.PointingHandCursor : Qt.ArrowCursor
        z: -1
        onClicked: {
            if (root.clickable) {
                root.clicked()
            }
        }
    }
}
