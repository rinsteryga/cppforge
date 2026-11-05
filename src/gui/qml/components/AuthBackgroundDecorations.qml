import QtQuick
import CppForge

Item {
    id: root

    anchors.fill: parent
    clip: true

    // Base opacity scale
    readonly property real cardOpacity: Theme.isDark ? 0.32 : 0.26
    readonly property real iconOpacity: Theme.isDark ? 0.08 : 0.06

    // ==========================================
    // TOP LEFT: Completed Lessons Card & Code Icon
    // ==========================================
    Item {
        id: groupTopLeft
        x: Math.max(20, root.width * 0.04)
        y: Math.max(40, root.height * 0.08)
        width: 170
        height: 100

        // Gentle floating animation
        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation { to: groupTopLeft.y - 6; duration: 4200; easing.type: Easing.InOutSine }
            NumberAnimation { to: groupTopLeft.y + 6; duration: 4200; easing.type: Easing.InOutSine }
        }

        // Mini decorative stat card (Completed Lessons)
        Rectangle {
            anchors.fill: parent
            radius: Theme.radiusMd
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderSubtle
            opacity: root.cardOpacity
            clip: true

            // Green top accent line
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 3
                color: Theme.success
            }

            // Watermark icon
            ForgeIcon {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: -12
                name: "book"
                size: 64
                color: Theme.success
                opacity: 0.18
            }

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Row {
                    spacing: 6
                    Rectangle {
                        width: 20
                        height: 20
                        radius: 3
                        color: Theme.successBg
                        ForgeIcon { anchors.centerIn: parent; name: "book"; size: 12; color: Theme.success }
                    }
                    Text {
                        text: "Completed Lessons"
                        font.family: Theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: Theme.textSecondary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Text {
                    text: "12 Lessons"
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: Theme.success
                }
            }
        }
    }

    // Large floating Code icon (Top Left outer)
    ForgeIcon {
        x: Math.max(10, root.width * 0.02)
        y: Math.max(160, root.height * 0.28)
        name: "code"
        size: 90
        color: Theme.accent
        opacity: root.iconOpacity
        rotation: -14

        SequentialAnimation on rotation {
            loops: Animation.Infinite
            NumberAnimation { to: -8; duration: 5000; easing.type: Easing.InOutSine }
            NumberAnimation { to: -14; duration: 5000; easing.type: Easing.InOutSine }
        }
    }

    // ==========================================
    // BOTTOM LEFT: Solved Tasks Card & Bulb Icon
    // ==========================================
    Item {
        id: groupBottomLeft
        x: Math.max(30, root.width * 0.06)
        y: Math.max(280, root.height * 0.62)
        width: 170
        height: 100

        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation { to: groupBottomLeft.y + 6; duration: 4600; easing.type: Easing.InOutSine }
            NumberAnimation { to: groupBottomLeft.y - 6; duration: 4600; easing.type: Easing.InOutSine }
        }

        Rectangle {
            anchors.fill: parent
            radius: Theme.radiusMd
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderSubtle
            opacity: root.cardOpacity
            clip: true

            // Blue top accent line
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 3
                color: Theme.accent
            }

            // Watermark icon
            ForgeIcon {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: -12
                name: "target"
                size: 64
                color: Theme.accent
                opacity: 0.18
            }

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Row {
                    spacing: 6
                    Rectangle {
                        width: 20
                        height: 20
                        radius: 3
                        color: Theme.accentLight
                        ForgeIcon { anchors.centerIn: parent; name: "target"; size: 12; color: Theme.accent }
                    }
                    Text {
                        text: "Solved Tasks"
                        font.family: Theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: Theme.textSecondary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Text {
                    text: "48 Solved"
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: Theme.accent
                }
            }
        }
    }

    // Floating Bulb / Tip icon (Bottom Left outer)
    ForgeIcon {
        x: Math.max(40, root.width * 0.08)
        y: Math.max(380, root.height * 0.82)
        name: "bulb"
        size: 74
        color: Theme.warning
        opacity: root.iconOpacity
        rotation: 12
    }

    // ==========================================
    // TOP RIGHT: Duel Points Card & Swords Icon
    // ==========================================
    Item {
        id: groupTopRight
        x: root.width - width - Math.max(20, root.width * 0.04)
        y: Math.max(40, root.height * 0.08)
        width: 170
        height: 100

        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation { to: groupTopRight.y + 6; duration: 4400; easing.type: Easing.InOutSine }
            NumberAnimation { to: groupTopRight.y - 6; duration: 4400; easing.type: Easing.InOutSine }
        }

        Rectangle {
            anchors.fill: parent
            radius: Theme.radiusMd
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderSubtle
            opacity: root.cardOpacity
            clip: true

            // Orange top accent line
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 3
                color: Theme.warning
            }

            // Watermark icon
            ForgeIcon {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: -12
                name: "swords"
                size: 64
                color: Theme.warning
                opacity: 0.18
            }

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Row {
                    spacing: 6
                    Rectangle {
                        width: 20
                        height: 20
                        radius: 3
                        color: Theme.warningBg
                        ForgeIcon { anchors.centerIn: parent; name: "swords"; size: 12; color: Theme.warning }
                    }
                    Text {
                        text: "Duel Points"
                        font.family: Theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: Theme.textSecondary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Text {
                    text: "1,250 PTS"
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: Theme.warning
                }
            }
        }
    }

    // Large floating Swords watermark icon (Top Right outer)
    ForgeIcon {
        x: root.width - width - Math.max(10, root.width * 0.02)
        y: Math.max(160, root.height * 0.28)
        name: "swords"
        size: 96
        color: Theme.warning
        opacity: root.iconOpacity
        rotation: 16

        SequentialAnimation on rotation {
            loops: Animation.Infinite
            NumberAnimation { to: 22; duration: 5200; easing.type: Easing.InOutSine }
            NumberAnimation { to: 16; duration: 5200; easing.type: Easing.InOutSine }
        }
    }

    // ==========================================
    // BOTTOM RIGHT: Duel Winrate Card & Trophy / Flame Icon
    // ==========================================
    Item {
        id: groupBottomRight
        x: root.width - width - Math.max(30, root.width * 0.06)
        y: Math.max(280, root.height * 0.62)
        width: 170
        height: 100

        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation { to: groupBottomRight.y - 6; duration: 4000; easing.type: Easing.InOutSine }
            NumberAnimation { to: groupBottomRight.y + 6; duration: 4000; easing.type: Easing.InOutSine }
        }

        Rectangle {
            anchors.fill: parent
            radius: Theme.radiusMd
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderSubtle
            opacity: root.cardOpacity
            clip: true

            // Accent top strip
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 3
                color: Theme.accent
            }

            // Watermark icon
            ForgeIcon {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: -12
                name: "trophy"
                size: 64
                color: Theme.accent
                opacity: 0.18
            }

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Row {
                    spacing: 6
                    Rectangle {
                        width: 20
                        height: 20
                        radius: 3
                        color: Theme.accentLight
                        ForgeIcon { anchors.centerIn: parent; name: "trophy"; size: 12; color: Theme.accent }
                    }
                    Text {
                        text: "Duel Winrate"
                        font.family: Theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: Theme.textSecondary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Text {
                    text: "84%"
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: Theme.textPrimary
                }
            }
        }
    }

    // Floating Streak Flame icon (Bottom Right outer)
    ForgeIcon {
        x: root.width - width - Math.max(40, root.width * 0.08)
        y: Math.max(380, root.height * 0.82)
        name: "fire"
        size: 78
        color: Theme.warning
        opacity: root.iconOpacity
        rotation: -10
    }
}
