import QtQuick
import CppForge

Rectangle {
    id: control

    property string text: ""
    property string variant: "neutral" // "accent", "success", "warning", "danger", "neutral"
    property int preferredHeight: 22

    implicitWidth: badgeText.implicitWidth + 14
    implicitHeight: preferredHeight
    radius: Theme.radiusSm

    color: {
        switch (control.variant) {
            case "accent": return Theme.accentLight
            case "success": return Theme.successBg
            case "warning": return Theme.warningBg
            case "danger": return Theme.dangerBg
            case "neutral":
            default:
                return Theme.bgCardHover
        }
    }

    border.width: 1
    border.color: {
        switch (control.variant) {
            case "accent": return Theme.accent
            case "success": return Theme.success
            case "warning": return Theme.warning
            case "danger": return Theme.danger
            case "neutral":
            default:
                return Theme.borderSubtle
        }
    }

    Text {
        id: badgeText
        anchors.centerIn: parent
        text: control.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSizeXs
        font.weight: Font.Bold
        color: {
            switch (control.variant) {
                case "accent": return Theme.accent
                case "success": return Theme.success
                case "warning": return Theme.warning
                case "danger": return Theme.danger
                case "neutral":
                default:
                    return Theme.textSecondary
            }
        }
    }
}
