import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var dashboardController

    property bool isInLobby: false
    property string lobbyCode: "FORGE-774"

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

            // 1. User League & Rank Bar (KDE Breeze style)
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                radius: Theme.radiusLg
                color: Theme.bgCard
                border.width: 1
                border.color: Theme.borderStrong

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20

                    // Avatar
                    Rectangle {
                        Layout.preferredWidth: 70
                        Layout.preferredHeight: 70
                        radius: 35
                        color: Theme.bgApp
                        clip: true

                        Image {
                            anchors.fill: parent
                            source: root.dashboardController ? root.dashboardController.avatarPath : "qrc:/images/default_avatar1.png"
                            fillMode: Image.PreserveAspectCrop
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: root.dashboardController ? root.dashboardController.username : "Player"
                            font.family: Theme.fontFamily
                            font.pixelSize: 20
                            font.weight: Font.Bold
                            color: Theme.textPrimary
                        }

                        Row {
                            spacing: 12
                            Text {
                                text: "⚔️ League: Diamond"
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                font.weight: Font.DemiBold
                                color: Theme.accent
                            }
                            Text {
                                text: "•"
                                color: Theme.textMuted
                            }
                            Text {
                                text: "PTS: " + (root.dashboardController ? root.dashboardController.duelPoints : 0)
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                color: Theme.textSecondary
                            }
                            Text {
                                text: "•"
                                color: Theme.textMuted
                            }
                            Text {
                                text: "Winrate: " + (root.dashboardController ? root.dashboardController.winrate.toFixed(0) : "0") + "%"
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                color: Theme.textSecondary
                            }
                        }
                    }
                }
            }

            // 2. Duel Actions
            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                ForgeButton {
                    Layout.fillWidth: true
                    implicitHeight: 36
                    text: root.isInLobby ? "LEAVE LOBBY" : "CREATE LOBBY"
                    variant: root.isInLobby ? "danger" : "secondary"
                    onClicked: root.isInLobby = !root.isInLobby
                }

                ForgeButton {
                    Layout.fillWidth: true
                    implicitHeight: 36
                    text: "JOIN LOBBY"
                    variant: "secondary"
                    enabled: !root.isInLobby
                    onClicked: root.isInLobby = true
                }

                ForgeButton {
                    Layout.fillWidth: true
                    implicitHeight: 36
                    text: "START DUEL"
                    variant: "primary"
                    enabled: root.isInLobby
                    onClicked: {
                        duelStatusText.text = "Searching for matchmaking opponent in diamond pool..."
                    }
                }
            }

            // 3. Lobby Card (KDE Breeze style)
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 280
                radius: Theme.radiusLg
                color: Theme.bgCard
                border.width: 1
                border.color: Theme.borderStrong

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 16

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: root.isInLobby ? "LOBBY: " + root.lobbyCode : "LOBBY PARTICIPANTS"
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.Bold
                            font.letterSpacing: 1.0
                            color: Theme.textPrimary
                        }
                        Item { Layout.fillWidth: true }
                        ForgeBadge {
                            visible: root.isInLobby
                            text: "2 / 2 READY"
                            variant: "success"
                        }
                    }

                    // Participant 1 (Me)
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        radius: Theme.radiusMd
                        color: Theme.bgApp
                        border.width: 1
                        border.color: Theme.borderSubtle

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            spacing: 14

                            Image {
                                Layout.preferredWidth: 36
                                Layout.preferredHeight: 36
                                source: root.dashboardController ? root.dashboardController.avatarPath : "qrc:/images/default_avatar1.png"
                                fillMode: Image.PreserveAspectCrop
                            }

                            Text {
                                Layout.fillWidth: true
                                text: (root.dashboardController ? root.dashboardController.username : "Developer") + " (You)"
                                font.family: Theme.fontFamily
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: Theme.textPrimary
                            }

                            Text {
                                text: "READY"
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                color: Theme.success
                            }
                        }
                    }

                    // Participant 2 (Opponent)
                    Rectangle {
                        visible: root.isInLobby
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        radius: Theme.radiusMd
                        color: Theme.bgApp
                        border.width: 1
                        border.color: Theme.borderSubtle

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            spacing: 14

                            Image {
                                Layout.preferredWidth: 36
                                Layout.preferredHeight: 36
                                source: "qrc:/images/walter.png"
                                fillMode: Image.PreserveAspectCrop
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "Walter_White (Opponent)"
                                font.family: Theme.fontFamily
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: Theme.textPrimary
                            }

                            Text {
                                text: "READY"
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                color: Theme.success
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    Text {
                        id: duelStatusText
                        Layout.fillWidth: true
                        text: root.isInLobby ? "Lobby created. Both participants are ready. Click START DUEL to begin battle." : "Create or join a duel lobby to compete in real-time C++ coding duels."
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        color: Theme.textSecondary
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
