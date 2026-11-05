import QtQuick
import QtQuick.Layouts
import CppForge

Rectangle {
    id: control

    property string title: "Metric"
    property string value: "0"
    property string detail: ""
    property string iconName: "code"
    property color valueColor: Theme.accent
    property color iconColor: control.valueColor
    property bool hasAccentTop: true

    implicitWidth: 160
    implicitHeight: 96
    radius: Theme.radiusMd
    color: Theme.bgCard
    border.width: 1
    border.color: Theme.borderSubtle
    clip: true

    // Top accent indicator line
    Rectangle {
        visible: control.hasAccentTop
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2
        color: control.valueColor
    }

    // Large background watermark icon for high visual polish
    ForgeIcon {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: -10
        name: control.iconName
        color: control.valueColor
        size: 64
        opacity: Theme.isDark ? 0.07 : 0.05
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            // Clean vector icon badge
            Rectangle {
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
                radius: Theme.radiusSm
                color: Theme.accentLight

                ForgeIcon {
                    anchors.centerIn: parent
                    name: control.iconName
                    color: control.iconColor
                    size: 13
                }
            }

            Text {
                Layout.fillWidth: true
                text: control.title
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSm
                color: Theme.textSecondary
                elide: Text.ElideRight
            }
        }

        Text {
            Layout.fillWidth: true
            text: control.value
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeXxl
            font.weight: Font.Bold
            color: control.valueColor
            elide: Text.ElideRight
        }

        Text {
            visible: control.detail !== ""
            Layout.fillWidth: true
            text: control.detail
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeXs
            color: Theme.textMuted
            elide: Text.ElideRight
        }
    }
}
