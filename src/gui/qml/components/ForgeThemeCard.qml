import QtQuick
import QtQuick.Layouts
import CppForge

Item {
    id: control

    property string title: "Theme"
    property bool isDarkTheme: false
    property bool isSelected: false

    signal selected()

    implicitWidth: 140
    implicitHeight: 110

    ColumnLayout {
        anchors.fill: parent
        spacing: 6

        // Theme Window Mockup Preview (like KDE System Settings)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.radiusMd
            color: control.isDarkTheme ? "#232629" : "#eff0f1"
            border.width: control.isSelected ? 2 : 1
            border.color: control.isSelected ? Theme.accent : (cardMouse.containsMouse ? Theme.borderStrong : Theme.borderSubtle)

            Behavior on border.color { ColorAnimation { duration: Theme.animFast } }

            // Mini window mockup inside
            Rectangle {
                anchors.fill: parent
                anchors.margins: 6
                radius: Theme.radiusSm
                color: control.isDarkTheme ? "#1b1e20" : "#ffffff"
                border.width: 1
                border.color: control.isDarkTheme ? "#31363b" : "#e0e2e4"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Mini Titlebar
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 14
                        color: control.isDarkTheme ? "#2a2e32" : "#fcfcfc"

                        Row {
                            anchors.right: parent.right
                            anchors.rightMargin: 4
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 3
                            Rectangle { width: 5; height: 5; radius: 2.5; color: Theme.accent }
                            Rectangle { width: 5; height: 5; radius: 2.5; color: Theme.warning }
                            Rectangle { width: 5; height: 5; radius: 2.5; color: Theme.danger }
                        }
                    }

                    // Mini Content (Sidebar + Main)
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 0

                        // Mini Sidebar
                        Rectangle {
                            Layout.preferredWidth: 26
                            Layout.fillHeight: true
                            color: control.isDarkTheme ? "#232629" : "#f5f6f7"

                            Column {
                                anchors.centerIn: parent
                                spacing: 3
                                Rectangle { width: 18; height: 3; radius: 1.5; color: control.isSelected ? Theme.accent : (control.isDarkTheme ? "#474a4c" : "#bdc3c7") }
                                Rectangle { width: 14; height: 3; radius: 1.5; color: control.isDarkTheme ? "#31363b" : "#e0e2e4" }
                                Rectangle { width: 16; height: 3; radius: 1.5; color: control.isDarkTheme ? "#31363b" : "#e0e2e4" }
                            }
                        }

                        // Mini Main View
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "transparent"

                            Column {
                                anchors.centerIn: parent
                                spacing: 4
                                Rectangle { width: 44; height: 6; radius: 2; color: Theme.accent }
                                Rectangle { width: 52; height: 4; radius: 2; color: control.isDarkTheme ? "#31363b" : "#e0e2e4" }
                                Rectangle { width: 36; height: 4; radius: 2; color: control.isDarkTheme ? "#31363b" : "#e0e2e4" }
                            }
                        }
                    }
                }
            }

            // Selection checkmark badge in bottom-right corner
            Rectangle {
                visible: control.isSelected
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 4
                width: 18
                height: 18
                radius: 9
                color: Theme.accent

                Text {
                    anchors.centerIn: parent
                    text: "✓"
                    font.pixelSize: 11
                    font.weight: Font.Bold
                    color: "white"
                }
            }
        }

        // Title Label
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: control.title
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeSm
            font.weight: control.isSelected ? Font.Bold : Font.Normal
            color: control.isSelected ? Theme.accent : Theme.textPrimary
        }
    }

    MouseArea {
        id: cardMouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: control.selected()
    }
}
