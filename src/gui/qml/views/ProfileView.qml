import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var dashboardController

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contentCol.implicitHeight + 40
        clip: true

        ColumnLayout {
            id: contentCol
            width: Math.min(parent.width - 48, 860)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 24
            spacing: 24

            // 1. User Header Card
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                radius: Theme.radiusLg
                color: Theme.bgCard
                border.width: 1
                border.color: Theme.borderStrong

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    // Avatar with hover edit overlay
                    Rectangle {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 80
                        radius: 40
                        color: Theme.bgApp
                        clip: true

                        Image {
                            anchors.fill: parent
                            source: root.dashboardController ? root.dashboardController.avatarPath : "qrc:/images/default_avatar1.png"
                            fillMode: Image.PreserveAspectCrop
                        }

                        Rectangle {
                            anchors.fill: parent
                            radius: 40
                            color: "#80000000"
                            visible: avatarMouse.containsMouse

                            Text {
                                anchors.centerIn: parent
                                text: "Edit"
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                color: "white"
                            }
                        }

                        MouseArea {
                            id: avatarMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: avatarModal.open()
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Text {
                            text: root.dashboardController ? root.dashboardController.username : "Developer"
                            font.family: Theme.fontFamily
                            font.pixelSize: 22
                            font.weight: Font.Bold
                            color: Theme.textPrimary
                        }

                        Row {
                            spacing: 8
                            Rectangle {
                                width: badgeText.implicitWidth + 14
                                height: 22
                                radius: 11
                                color: Theme.accentLight

                                Text {
                                    id: badgeText
                                    anchors.centerIn: parent
                                    text: "C++ Explorer"
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 11
                                    font.weight: Font.DemiBold
                                    color: Theme.accent
                                }
                            }

                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 6

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "Streak: " + (root.dashboardController ? root.dashboardController.streak : 0) + " days"
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 12
                                    color: Theme.textSecondary
                                }

                                ForgeIcon {
                                    anchors.verticalCenter: parent.verticalCenter
                                    name: "fire"
                                    color: Theme.warning
                                    size: 14
                                }
                            }
                        }
                    }

                    ForgeButton {
                        text: "Change Avatar"
                        variant: "secondary"
                        onClicked: avatarModal.open()
                    }
                }
            }

            // 2. Achievements Showcase (Horizontal Row in Upper Part)
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 145
                radius: Theme.radiusLg
                color: Theme.bgCard
                border.width: 1
                border.color: Theme.borderStrong

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Achievements"
                            font.family: Theme.fontFamily
                            font.pixelSize: 16
                            font.weight: Font.Bold
                            color: Theme.textPrimary
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: {
                                var achList = root.dashboardController ? root.dashboardController.achievements : []
                                var unlocked = 0
                                for (var i = 0; i < achList.length; i++) {
                                    if (achList[i].isUnlocked) unlocked++
                                }
                                return unlocked + " of " + achList.length + " unlocked"
                            }
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.DemiBold
                            color: Theme.textSecondary
                        }
                    }

                    // Horizontal Row of Achievements
                    Flickable {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        contentWidth: achRow.implicitWidth
                        contentHeight: height
                        flickableDirection: Flickable.HorizontalFlick
                        boundsBehavior: Flickable.StopAtBounds
                        clip: false

                        Row {
                            id: achRow
                            spacing: 18
                            anchors.verticalCenter: parent.verticalCenter

                            Repeater {
                                model: root.dashboardController ? root.dashboardController.achievements : []

                                delegate: Item {
                                    id: achItem
                                    width: 76
                                    height: 78

                                    ColumnLayout {
                                        anchors.centerIn: parent
                                        spacing: 6

                                        // Icon circle with status styling
                                        Rectangle {
                                            Layout.preferredWidth: 52
                                            Layout.preferredHeight: 52
                                            Layout.alignment: Qt.AlignHCenter
                                            radius: 26
                                            color: modelData.isUnlocked ? (Theme.isDark ? "#2d2415" : "#fef3c7") : (Theme.isDark ? "#232629" : "#eef0f2")
                                            border.width: modelData.isUnlocked ? 2 : 1
                                            border.color: modelData.isUnlocked ? "#f59e0b" : Theme.borderSubtle
                                            scale: achMouse.containsMouse ? 1.12 : 1.0
                                            Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

                                            Image {
                                                anchors.centerIn: parent
                                                width: 36
                                                height: 36
                                                source: modelData.iconPath
                                                fillMode: Image.PreserveAspectFit
                                                opacity: modelData.isUnlocked ? 1.0 : 0.35
                                            }

                                            // Small lock overlay badge if locked
                                            Rectangle {
                                                visible: !modelData.isUnlocked
                                                anchors.bottom: parent.bottom
                                                anchors.right: parent.right
                                                width: 18
                                                height: 18
                                                radius: 9
                                                color: Theme.isDark ? "#1b1e20" : "#ffffff"
                                                border.width: 1
                                                border.color: Theme.borderStrong

                                                ForgeLockIcon {
                                                    anchors.centerIn: parent
                                                    size: 10
                                                    color: Theme.textMuted
                                                    holeColor: Theme.isDark ? "#1b1e20" : "#ffffff"
                                                }
                                            }
                                        }

                                        // Short title
                                        Text {
                                            Layout.fillWidth: true
                                            text: modelData.name
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 11
                                            font.weight: Font.DemiBold
                                            color: modelData.isUnlocked ? Theme.textPrimary : Theme.textMuted
                                            horizontalAlignment: Text.AlignHCenter
                                            elide: Text.ElideRight
                                        }
                                    }

                                    // Interactive Hover Area with Rich ToolTip
                                    MouseArea {
                                        id: achMouse
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        cursorShape: Qt.PointingHandCursor
                                    }

                                    ToolTip {
                                        id: achTip
                                        visible: achMouse.containsMouse
                                        delay: 150
                                        timeout: 6000
                                        y: -contentItem.implicitHeight - 12
                                        x: (parent.width - contentItem.implicitWidth) / 2

                                        contentItem: ColumnLayout {
                                            spacing: 4

                                            RowLayout {
                                                spacing: 8

                                                Text {
                                                    text: modelData.name
                                                    font.family: Theme.fontFamily
                                                    font.pixelSize: 13
                                                    font.weight: Font.Bold
                                                    color: Theme.textPrimary
                                                }

                                                Rectangle {
                                                    Layout.preferredHeight: 18
                                                    Layout.preferredWidth: tipStatusLbl.implicitWidth + 8
                                                    radius: 9
                                                    color: modelData.isUnlocked ? Theme.successBg : (Theme.isDark ? "#232629" : "#eef0f2")

                                                    Text {
                                                        id: tipStatusLbl
                                                        anchors.centerIn: parent
                                                        text: modelData.isUnlocked ? "✓ Unlocked" : "Locked"
                                                        font.family: Theme.fontFamily
                                                        font.pixelSize: 10
                                                        font.weight: Font.Bold
                                                        color: modelData.isUnlocked ? Theme.success : Theme.textMuted
                                                    }
                                                }
                                            }

                                            Text {
                                                text: modelData.description
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                color: Theme.textSecondary
                                                wrapMode: Text.WordWrap
                                                Layout.maximumWidth: 260
                                            }
                                        }

                                        background: Rectangle {
                                            color: Theme.bgCard
                                            border.width: 1
                                            border.color: Theme.borderStrong
                                            radius: Theme.radiusMd
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 3. Statistics Grid
            Text {
                text: "Statistics"
                font.family: Theme.fontFamily
                font.pixelSize: 18
                font.weight: Font.Bold
                color: Theme.textPrimary
            }

            GridLayout {
                Layout.fillWidth: true
                columns: width > 600 ? 4 : 2
                columnSpacing: 16
                rowSpacing: 16

                ForgeStatCard {
                    Layout.fillWidth: true
                    title: "Solved Tasks"
                    value: "" + (root.dashboardController ? root.dashboardController.solvedTasksCount : 0)
                    iconName: "target"
                    valueColor: Theme.accent
                }

                ForgeStatCard {
                    Layout.fillWidth: true
                    title: "Completed Lessons"
                    value: "" + (root.dashboardController ? root.dashboardController.completedLessonsCount : 0)
                    iconName: "book"
                    valueColor: Theme.success
                }

                ForgeStatCard {
                    Layout.fillWidth: true
                    title: "Duel Points"
                    value: "" + (root.dashboardController ? root.dashboardController.duelPoints : 0) + " PTS"
                    iconName: "duels"
                    valueColor: Theme.warning
                }

                ForgeStatCard {
                    Layout.fillWidth: true
                    title: "Duel Winrate"
                    value: "" + (root.dashboardController ? root.dashboardController.winrate.toFixed(0) : "0") + "%"
                    iconName: "trophy"
                    valueColor: Theme.textPrimary
                }
            }

            // 3. Settings Card (Theme Previews & Toggles)
            Text {
                text: "Appearance & Settings"
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeXl
                font.weight: Font.Bold
                color: Theme.textPrimary
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: settingsCol.implicitHeight + 32
                radius: Theme.radiusLg
                color: Theme.bgCard
                border.width: 1
                border.color: Theme.borderSubtle

                ColumnLayout {
                    id: settingsCol
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16

                    // Theme selector (Quick Settings style)
                    ColumnLayout {
                        spacing: 8
                        Text {
                            text: "Interface Theme"
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Font.Bold
                            color: Theme.textPrimary
                        }
                        Text {
                            text: "Select your preferred KDE Breeze visual theme (automatically saved to QSettings)"
                            font.family: Theme.fontFamily
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }

                        RowLayout {
                            spacing: 16
                            ForgeThemeCard {
                                title: "Breeze Light"
                                isDarkTheme: false
                                isSelected: !Theme.isDark
                                onSelected: Theme.setDark(false)
                            }

                            ForgeThemeCard {
                                title: "Breeze Dark"
                                isDarkTheme: true
                                isSelected: Theme.isDark
                                onSelected: Theme.setDark(true)
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme.divider
                    }

                    // Toggles section
                    RowLayout {
                        Layout.fillWidth: true
                        ColumnLayout {
                            spacing: 2
                            Text {
                                text: "Editor Animations"
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeMd
                                font.weight: Font.Medium
                                color: Theme.textPrimary
                            }
                            Text {
                                text: "Smooth transitions across tabs, gauges and navigation"
                                font.family: Theme.fontFamily
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.textSecondary
                            }
                        }
                        Item { Layout.fillWidth: true }
                        ForgeSwitch {
                            checked: true
                        }
                    }
                }
            }

            // App Appearance ended
        }
    }

    // Avatar Selection Dialog Modal
    Popup {
        id: avatarModal
        anchors.centerIn: parent
        width: 440
        height: 380
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            radius: 14
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderStrong
        }

        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: "Select Profile Avatar"
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                    color: Theme.textPrimary
                }
                Item { Layout.fillWidth: true }
                ForgeButton {
                    text: "✕"
                    variant: "ghost"
                    implicitWidth: 32
                    implicitHeight: 32
                    onClicked: avatarModal.close()
                }
            }

            GridView {
                id: avatarGrid
                Layout.fillWidth: true
                Layout.fillHeight: true
                cellWidth: 74
                cellHeight: 74
                clip: true

                model: root.dashboardController ? root.dashboardController.availableAvatars : []

                delegate: Rectangle {
                    width: 64
                    height: 64
                    radius: 32
                    color: "transparent"
                    border.width: (root.dashboardController && root.dashboardController.avatarPath === modelData) ? 3 : 1
                    border.color: (root.dashboardController && root.dashboardController.avatarPath === modelData) ? Theme.accent : Theme.borderSubtle

                    Image {
                        anchors.fill: parent
                        anchors.margins: 4
                        source: modelData
                        fillMode: Image.PreserveAspectCrop
                        clip: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (root.dashboardController) {
                                root.dashboardController.updateAvatar(modelData)
                            }
                            avatarModal.close()
                        }
                    }
                }
            }
        }
    }
}
