import QtQuick
import QtQuick.Controls.Basic
import CppForge

Button {
    id: control

    // Variants: "primary", "secondary", "ghost", "danger"
    property string variant: "primary"
    property string iconSource: ""
    property int iconSize: 16
    property bool loading: false

    implicitHeight: 34
    implicitWidth: Math.max(contentRow.implicitWidth + Theme.spacingLg * 2, 84)

    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSizeMd
    font.weight: Font.Medium

    hoverEnabled: true

    HoverHandler {
        cursorShape: control.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    contentItem: Item {
        width: control.availableWidth
        height: control.availableHeight
        implicitWidth: contentRow.implicitWidth
        implicitHeight: contentRow.implicitHeight

        Row {
            id: contentRow
            anchors.centerIn: parent
            spacing: Theme.spacingSm

            Image {
                id: btnIcon
                source: control.iconSource
                visible: control.iconSource !== "" && !control.loading
                width: control.iconSize
                height: control.iconSize
                sourceSize.width: control.iconSize
                sourceSize.height: control.iconSize
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }

            // Simple spinner when loading
            Rectangle {
                id: spinner
                visible: control.loading
                width: 14
                height: 14
                radius: 7
                color: "transparent"
                border.color: control.textColor
                border.width: 2
                anchors.verticalCenter: parent.verticalCenter

                RotationAnimation on rotation {
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 900
                    running: control.loading
                }
            }

            Text {
                id: btnText
                text: control.text
                font: control.font
                color: control.textColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    readonly property color textColor: {
        if (!control.enabled) return Theme.textMuted
        if (control.variant === "primary") return Theme.textOnAccent
        if (control.variant === "danger") return Theme.textOnAccent
        if (control.variant === "ghost") return control.hovered ? Theme.accent : Theme.textPrimary
        return Theme.textPrimary
    }

    readonly property color buttonBg: {
        if (!control.enabled) {
            return control.variant === "ghost" ? "transparent" : (Theme.isDark ? "#2a2e32" : "#e0e2e4")
        }
        if (control.variant === "primary") {
            if (control.down) return Theme.accentPressed
            if (control.hovered) return Theme.accentHover
            return Theme.accent
        }
        if (control.variant === "danger") {
            if (control.down) return Theme.dangerHover
            if (control.hovered) return Theme.dangerHover
            return Theme.danger
        }
        if (control.variant === "ghost") {
            if (control.down) return Theme.isDark ? "#292c2f" : "#d3d7da"
            if (control.hovered) return Theme.isDark ? "#31363b" : "#e0e2e4"
            return "transparent"
        }
        // "secondary" (standard Breeze button)
        if (control.down) return Theme.isDark ? "#292c2f" : "#e0e2e4"
        if (control.hovered) return Theme.isDark ? "#3a3f44" : "#ffffff"
        return Theme.isDark ? "#31363b" : "#fcfcfc"
    }

    readonly property color buttonBorder: {
        if (!control.enabled) return "transparent"
        if (control.variant === "primary") return control.hovered ? Theme.accentHover : Theme.accentPressed
        if (control.variant === "danger") return control.hovered ? Theme.dangerHover : Theme.danger
        if (control.variant === "ghost") return control.hovered ? Theme.borderStrong : "transparent"
        // Breeze secondary: border lights up in Breeze Blue on hover
        return control.hovered ? Theme.accent : Theme.borderStrong
    }

    background: Rectangle {
        implicitWidth: 84
        implicitHeight: 34
        radius: Theme.radiusMd
        color: control.buttonBg
        border.color: control.buttonBorder
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
        Behavior on border.color {
            ColorAnimation { duration: Theme.animFast }
        }
    }
}
