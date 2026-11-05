import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var taskController
    signal backRequested()

    property int activeTab: 1 // 0: Theory, 1: Practice

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 14

        // 1. Top bar
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            ForgeButton {
                text: "← Back"
                variant: "secondary"
                implicitHeight: 34
                onClicked: root.backRequested()
            }

            Text {
                text: root.taskController ? root.taskController.title : "Coding Task"
                font.family: Theme.fontFamily
                font.pixelSize: 18
                font.weight: Font.Bold
                color: Theme.textPrimary
            }

            ForgeBadge {
                visible: root.taskController && root.taskController.isCompleted
                text: "✓ Solved"
                variant: "success"
            }

            Item { Layout.fillWidth: true }
        }

        // 2. Split Area (Left: Theory/Practice, Right: Editor + Console)
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            // LEFT PANE: Theory / Practice Tabs (KDE Breeze style)
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 420
                radius: Theme.radiusLg
                color: Theme.bgCard
                border.width: 1
                border.color: Theme.borderStrong
                clip: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Segmented Tab Switcher (KDE Breeze style)
                    Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 46

                        ForgeSegmentedControl {
                            anchors.fill: parent
                            anchors.margins: 7
                            model: ["Theory", "Practice"]
                            currentIndex: root.activeTab
                            onIndexSelected: function(idx) {
                                root.activeTab = idx
                            }
                        }
                    }

                    // Content
                    Flickable {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        contentWidth: width
                        contentHeight: tabText.implicitHeight + 40
                        clip: true

                        Text {
                            id: tabText
                            anchors.fill: parent
                            anchors.margins: 20
                            text: root.activeTab === 0 ? (root.taskController ? root.taskController.theory : "") : (root.taskController ? root.taskController.description : "")
                            textFormat: Text.MarkdownText
                            font.family: Theme.fontFamily
                            font.pixelSize: 13
                            lineHeight: 1.5
                            wrapMode: Text.WordWrap
                            color: Theme.textPrimary
                        }
                    }
                }
            }

            // RIGHT PANE: Code Editor & Terminal Output
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12

                // Editor Box (KDE Breeze style)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: Theme.radiusLg
                    color: Theme.bgCode
                    border.width: 1
                    border.color: Theme.borderStrong
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        // Editor Header
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 34
                            color: Theme.isDark ? "#2a2e32" : "#fcfcfc"

                            Rectangle {
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 1
                                color: Theme.borderStrong
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 16
                                anchors.rightMargin: 16

                                Text {
                                    text: "main.cpp"
                                    font.family: Theme.fontCode
                                    font.pixelSize: Theme.fontSizeSm
                                    font.weight: Font.DemiBold
                                    color: Theme.textPrimary
                                }

                                Item { Layout.fillWidth: true }

                                ForgeButton {
                                    text: "Reset"
                                    variant: "ghost"
                                    implicitHeight: 26
                                    onClicked: {
                                        if (root.taskController) {
                                            codeArea.text = root.taskController.initialCode
                                        }
                                    }
                                }
                            }
                        }

                        // Code input area
                        Flickable {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            contentWidth: width
                            contentHeight: codeArea.implicitHeight + 20
                            clip: true

                            TextArea {
                                id: codeArea
                                anchors.fill: parent
                                anchors.margins: 12
                                text: root.taskController ? root.taskController.initialCode : ""
                                font.family: Theme.fontCode
                                font.pixelSize: Theme.fontSizeMd
                                color: Theme.textPrimary
                                wrapMode: TextEdit.NoWrap
                                selectByMouse: true
                                background: null
                                tabStopDistance: 32
                            }
                        }
                    }
                }

                // Output / Terminal Box (KDE Breeze Konsole style)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    radius: Theme.radiusLg
                    color: Theme.isDark ? "#1b1e20" : "#232629"
                    border.width: 1
                    border.color: Theme.borderStrong
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        // Console Header & Action Buttons
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            color: Theme.isDark ? "#232629" : "#31363b"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 14
                                anchors.rightMargin: 14

                                Text {
                                    text: "Terminal Output"
                                    font.family: Theme.fontCode
                                    font.pixelSize: Theme.fontSizeXs
                                    font.weight: Font.Bold
                                    color: "#bdc3c7"
                                }

                                Item { Layout.fillWidth: true }

                                ForgeButton {
                                    text: "Clear"
                                    variant: "ghost"
                                    implicitHeight: 26
                                    onClicked: {
                                        if (root.taskController) root.taskController.clearLog()
                                    }
                                }

                                ForgeButton {
                                    text: "▶ Run"
                                    variant: "secondary"
                                    implicitHeight: 28
                                    loading: root.taskController ? root.taskController.isRunning : false
                                    onClicked: {
                                        if (root.taskController) root.taskController.runCode(codeArea.text)
                                    }
                                }

                                ForgeButton {
                                    text: "✓ Submit"
                                    variant: "primary"
                                    implicitHeight: 28
                                    loading: root.taskController ? root.taskController.isRunning : false
                                    onClicked: {
                                        if (root.taskController) root.taskController.submitSolution(codeArea.text)
                                    }
                                }
                            }
                        }

                        // Terminal Text Log
                        Flickable {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            contentWidth: width
                            contentHeight: termText.implicitHeight + 10
                            clip: true

                            Text {
                                id: termText
                                anchors.fill: parent
                                anchors.margins: 12
                                text: root.taskController ? (root.taskController.outputLog !== "" ? root.taskController.outputLog : "Ready. Click 'Run' to test solution.") : "Ready."
                                font.family: Theme.fontCode
                                font.pixelSize: 12
                                color: "#e2e8f0"
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
    }
}
