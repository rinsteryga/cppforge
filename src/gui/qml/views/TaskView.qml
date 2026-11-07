import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var taskController
    signal backRequested()

    property int activeTab: (root.taskController && root.taskController.hasCodingTask) ? 1 : 0 // 0: Theory, 1: Practice

    Connections {
        target: root.taskController ? root.taskController : null
        function onTaskChanged() {
            if (root.taskController) {
                codeArea.text = root.taskController.initialCode
                if (!root.taskController.hasCodingTask) {
                    root.activeTab = 0
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 14

        // 1. Top bar with navigation
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            ForgeButton {
                text: "← Back"
                variant: "secondary"
                implicitHeight: 34
                onClicked: root.backRequested()
            }

            ForgeButton {
                text: "◀ Prev"
                variant: "ghost"
                implicitHeight: 34
                enabled: root.taskController ? root.taskController.hasPrevLesson : false
                opacity: enabled ? 1.0 : 0.4
                onClicked: {
                    if (root.taskController) root.taskController.prevLesson()
                }
            }

            ForgeButton {
                text: "Next ▶"
                variant: "ghost"
                implicitHeight: 34
                enabled: root.taskController ? root.taskController.hasNextLesson : false
                opacity: enabled ? 1.0 : 0.4
                onClicked: {
                    if (root.taskController) root.taskController.nextLesson()
                }
            }

            Text {
                text: root.taskController ? root.taskController.title : "Coding Task"
                font.family: Theme.fontFamily
                font.pixelSize: 18
                font.weight: Font.Bold
                color: Theme.textPrimary
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            ForgeBadge {
                visible: root.taskController && root.taskController.isCompleted
                text: "✓ Completed"
                variant: "success"
            }
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
                            model: (root.taskController && root.taskController.hasCodingTask) ? ["Theory", "Practice"] : ["Theory"]
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
                            font.pixelSize: Theme.fontSizeMd
                            lineHeight: 1.6
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
                                    text: (root.taskController && !root.taskController.hasCodingTask) ? "Sandbox (main.cpp)" : "main.cpp"
                                    font.family: Theme.fontCode
                                    font.pixelSize: Theme.fontSizeSm
                                    font.weight: Font.DemiBold
                                    color: Theme.textPrimary
                                }

                                Item { Layout.fillWidth: true }

                                ForgeButton {
                                    visible: root.taskController ? root.taskController.hasCodingTask : false
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

                        // Code input area with Line Numbers
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 0

                            // Gutter: Line Numbers
                            Rectangle {
                                Layout.fillHeight: true
                                Layout.preferredWidth: 44
                                color: Theme.isDark ? "#1f2225" : "#f5f6f7"

                                Rectangle {
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    width: 1
                                    color: Theme.borderStrong
                                }

                                Text {
                                    anchors.fill: parent
                                    anchors.topMargin: 12
                                    anchors.rightMargin: 10
                                    horizontalAlignment: Text.AlignRight
                                    font.family: Theme.fontCode
                                    font.pixelSize: Theme.fontSizeSm
                                    color: Theme.textMuted
                                    text: {
                                        var count = Math.max(1, codeArea.lineCount)
                                        var out = ""
                                        for (var i = 1; i <= count; i++) {
                                            out += i + (i < count ? "\n" : "")
                                        }
                                        return out
                                    }
                                }
                            }

                            // Text Editor
                            Flickable {
                                id: flickEditor
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                contentWidth: width
                                contentHeight: codeArea.implicitHeight + 24
                                clip: true

                                TextArea {
                                    id: codeArea
                                    width: flickEditor.width
                                    topPadding: 12
                                    bottomPadding: 12
                                    leftPadding: 14
                                    rightPadding: 14
                                    text: root.taskController ? root.taskController.initialCode : ""
                                    font.family: Theme.fontCode
                                    font.pixelSize: Theme.fontSizeSm
                                    color: Theme.textPrimary
                                    readOnly: false
                                    wrapMode: TextEdit.NoWrap
                                    selectByMouse: true
                                    background: null
                                    tabStopDistance: 32

                                    QmlCppHighlighter {
                                        textDocument: codeArea.textDocument
                                        isDark: Theme.isDark
                                    }

                                    Keys.onPressed: function(event) {
                                        if (codeArea.readOnly) return

                                        // 1. Tab -> 4 spaces
                                        if (event.key === Qt.Key_Tab) {
                                            event.accepted = true
                                            var pos = codeArea.cursorPosition
                                            codeArea.insert(pos, "    ")
                                            codeArea.cursorPosition = pos + 4
                                            return
                                        }

                                        // 2. Auto-indent on Enter
                                        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                                            event.accepted = true
                                            var curPos = codeArea.cursorPosition
                                            var textBefore = codeArea.text.substring(0, curPos)
                                            var lastNewline = textBefore.lastIndexOf("\n")
                                            var currentLine = textBefore.substring(lastNewline + 1)

                                            var indentMatch = currentLine.match(/^(\s*)/)
                                            var indent = indentMatch ? indentMatch[1] : ""

                                            var trimmed = currentLine.trim()
                                            var textAfter = codeArea.text.substring(curPos)
                                            var startsWithCloseBrace = textAfter.trim().startsWith("}")

                                            if (trimmed.endsWith("{")) {
                                                if (startsWithCloseBrace) {
                                                    var blockIndent = "\n" + indent + "    \n" + indent
                                                    codeArea.insert(curPos, blockIndent)
                                                    codeArea.cursorPosition = curPos + indent.length + 5
                                                    return
                                                } else {
                                                    var singleIndent = "\n" + indent + "    "
                                                    codeArea.insert(curPos, singleIndent)
                                                    codeArea.cursorPosition = curPos + singleIndent.length
                                                    return
                                                }
                                            } else {
                                                var standardIndent = "\n" + indent
                                                codeArea.insert(curPos, standardIndent)
                                                codeArea.cursorPosition = curPos + standardIndent.length
                                                return
                                            }
                                        }

                                        // 3. Backspace between paired symbols
                                        if (event.key === Qt.Key_Backspace) {
                                            var bPos = codeArea.cursorPosition
                                            if (bPos > 0 && bPos < codeArea.text.length) {
                                                var prevChar = codeArea.text.charAt(bPos - 1)
                                                var nextChar = codeArea.text.charAt(bPos)
                                                if ((prevChar === "(" && nextChar === ")") ||
                                                    (prevChar === "[" && nextChar === "]") ||
                                                    (prevChar === "{" && nextChar === "}") ||
                                                    (prevChar === "\"" && nextChar === "\"") ||
                                                    (prevChar === "'" && nextChar === "'")) {
                                                    event.accepted = true
                                                    codeArea.remove(bPos - 1, bPos + 1)
                                                    codeArea.cursorPosition = bPos - 1
                                                    return
                                                }
                                            }
                                        }

                                        // 4. Overwrite closing symbol if already in front of it
                                        var closers = [")", "]", "}", "\"", "'"]
                                        if (closers.indexOf(event.text) !== -1) {
                                            var cPos = codeArea.cursorPosition
                                            if (cPos < codeArea.text.length && codeArea.text.charAt(cPos) === event.text) {
                                                event.accepted = true
                                                codeArea.cursorPosition = cPos + 1
                                                return
                                            }
                                        }

                                        // 5. Auto-closing pairs and selection wrapping
                                        var pairs = {
                                            "(": ")",
                                            "[": "]",
                                            "{": "}",
                                            "\"": "\"",
                                            "'": "'"
                                        }

                                        if (event.text in pairs) {
                                            var closeChar = pairs[event.text]
                                            var selStart = Math.min(codeArea.selectionStart, codeArea.selectionEnd)
                                            var selEnd = Math.max(codeArea.selectionStart, codeArea.selectionEnd)

                                            if (selStart !== selEnd) {
                                                event.accepted = true
                                                var selected = codeArea.selectedText
                                                codeArea.remove(selStart, selEnd)
                                                codeArea.insert(selStart, event.text + selected + closeChar)
                                                codeArea.select(selStart + 1, selStart + 1 + selected.length)
                                                return
                                            } else {
                                                event.accepted = true
                                                var pPos = codeArea.cursorPosition
                                                codeArea.insert(pPos, event.text + closeChar)
                                                codeArea.cursorPosition = pPos + 1
                                                return
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Output / Terminal Box (Adapts cleanly to dark and light themes)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 180
                    radius: Theme.radiusLg
                    color: Theme.isDark ? "#1b1e20" : "#f8f9fa"
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
                            color: Theme.isDark ? "#232629" : "#eef0f2"

                            Rectangle {
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 1
                                color: Theme.borderSubtle
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 14
                                anchors.rightMargin: 14

                                Text {
                                    text: "Terminal Output"
                                    font.family: Theme.fontCode
                                    font.pixelSize: Theme.fontSizeXs
                                    font.weight: Font.Bold
                                    color: Theme.isDark ? "#bdc3c7" : Theme.textSecondary
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

                                // If Theory Only: button "Mark Studied"
                                ForgeButton {
                                    visible: root.taskController ? !root.taskController.hasCodingTask : false
                                    text: (root.taskController && root.taskController.isCompleted) ? "✓ Studied" : "Mark as Studied"
                                    variant: "primary"
                                    implicitHeight: 28
                                    onClicked: {
                                        if (root.taskController) root.taskController.markTheoryStudied()
                                    }
                                }

                                // Code Sandbox / Runner: always available
                                ForgeButton {
                                    visible: true
                                    text: "▶ Run"
                                    variant: "secondary"
                                    implicitHeight: 28
                                    loading: root.taskController ? root.taskController.isRunning : false
                                    onClicked: {
                                        if (root.taskController) root.taskController.runCode(codeArea.text)
                                    }
                                }

                                ForgeButton {
                                    visible: root.taskController ? root.taskController.hasCodingTask : false
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
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.isDark ? "#e2e8f0" : Theme.textPrimary
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
    }
}
