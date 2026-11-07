import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var dashboardController
    signal backRequested()
    signal lessonSelected(int lessonId)

    readonly property var nodes: root.dashboardController ? root.dashboardController.roadmapNodes : []
    readonly property int nodeSpacing: 130
    readonly property int nodeRadius: 28
    readonly property int startY: 60

    function getNodeX(index, totalWidth) {
        var cx = totalWidth / 2
        var maxAmp = Math.max(50, Math.min(totalWidth * 0.18, 140))
        return cx + Math.sin(index * 1.15) * maxAmp
    }

    function getNodeY(index) {
        return startY + index * nodeSpacing
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        // 1. Navigation & Header bar
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            ForgeButton {
                text: "← Back to Modules"
                variant: "secondary"
                implicitHeight: 34
                onClicked: root.backRequested()
            }

            ColumnLayout {
                spacing: 2

                Text {
                    text: root.dashboardController ? root.dashboardController.currentModuleTitle : "Module Roadmap"
                    font.family: Theme.fontFamily
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    color: Theme.textPrimary
                }

                Text {
                    text: {
                        var c = 0
                        for (var i = 0; i < root.nodes.length; i++) {
                            if (root.nodes[i].isCompleted) c++
                        }
                        return c + " of " + root.nodes.length + " lessons completed"
                    }
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeSm
                    color: Theme.textSecondary
                }
            }

            Item { Layout.fillWidth: true }

            // Progress Pill Badge
            Rectangle {
                Layout.preferredHeight: 32
                Layout.preferredWidth: progText.implicitWidth + 24
                radius: 16
                color: Theme.isDark ? "#232629" : "#eef0f2"
                border.width: 1
                border.color: Theme.borderSubtle

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 8

                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        color: Theme.success
                    }

                    Text {
                        id: progText
                        text: {
                            var comp = 0
                            for (var i = 0; i < root.nodes.length; i++) {
                                if (root.nodes[i].isCompleted) comp++
                            }
                            var pct = root.nodes.length > 0 ? Math.round((comp / root.nodes.length) * 100) : 0
                            return pct + "% Progress"
                        }
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Bold
                        color: Theme.textPrimary
                    }
                }
            }
        }

        // 2. Interactive Serpentine Roadmap Canvas
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.radiusLg
            color: Theme.bgCard
            border.width: 1
            border.color: Theme.borderStrong
            clip: true

            Flickable {
                id: flick
                anchors.fill: parent
                contentWidth: width
                contentHeight: Math.max(height, root.nodes.length * root.nodeSpacing + 120)
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                // Background Curving Track (HTML5 Canvas)
                Canvas {
                    id: pathCanvas
                    anchors.fill: parent
                    z: 1

                    onWidthChanged: requestPaint()
                    onHeightChanged: requestPaint()

                    Connections {
                        target: root
                        function onNodesChanged() { pathCanvas.requestPaint() }
                    }
                    Connections {
                        target: Theme
                        function onIsDarkChanged() { pathCanvas.requestPaint() }
                    }

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.reset()
                        ctx.clearRect(0, 0, width, height)

                        var count = root.nodes.length
                        if (count < 2) return

                        // Pass 1: Base Inactive Track
                        ctx.lineWidth = 10
                        ctx.lineCap = "round"
                        ctx.lineJoin = "round"
                        ctx.strokeStyle = Theme.isDark ? "#2a2e32" : "#e2e8f0"

                        ctx.beginPath()
                        ctx.moveTo(root.getNodeX(0, width), root.getNodeY(0))

                        for (var i = 1; i < count; i++) {
                            var xPrev = root.getNodeX(i - 1, width)
                            var yPrev = root.getNodeY(i - 1)
                            var xCur = root.getNodeX(i, width)
                            var yCur = root.getNodeY(i)
                            var cy = (yPrev + yCur) / 2
                            ctx.bezierCurveTo(xPrev, cy, xCur, cy, xCur, yCur)
                        }
                        ctx.stroke()

                        // Pass 2: Active / Completed Segment Highlights
                        for (var j = 1; j < count; j++) {
                            var prevNode = root.nodes[j - 1]
                            if (prevNode && prevNode.isCompleted) {
                                var px0 = root.getNodeX(j - 1, width)
                                var py0 = root.getNodeY(j - 1)
                                var px1 = root.getNodeX(j, width)
                                var py1 = root.getNodeY(j)
                                var pcy = (py0 + py1) / 2

                                ctx.lineWidth = 10
                                ctx.strokeStyle = Theme.success
                                ctx.beginPath()
                                ctx.moveTo(px0, py0)
                                ctx.bezierCurveTo(px0, pcy, px1, pcy, px1, py1)
                                ctx.stroke()
                            }
                        }
                    }
                }

                // Node items & Attached Cards
                Repeater {
                    model: root.nodes

                    delegate: Item {
                        id: nodeItem
                        z: 2

                        readonly property real nx: root.getNodeX(index, flick.width)
                        readonly property real ny: root.getNodeY(index)
                        readonly property bool isRight: nx >= (flick.width / 2)
                        readonly property real cardW: Math.min(300, Math.max(220, flick.width * 0.35))
                        readonly property bool isCurrent: !modelData.isCompleted && !modelData.isLocked

                        readonly property bool isHovered: !modelData.isLocked && (circleMouse.containsMouse || cardMouse.containsMouse)

                        width: flick.width
                        height: root.nodeSpacing

                        // 1. Dash connector line between node and card
                        Rectangle {
                            y: nodeItem.ny - 1
                            x: nodeItem.isRight ? (nodeItem.nx - 30 - 24) : (nodeItem.nx + 30)
                            width: 24
                            height: 2
                            color: nodeItem.isHovered ? Theme.accent : (modelData.isCompleted ? Theme.success : Theme.borderStrong)
                        }

                        // 2. Floating Info Card
                        Rectangle {
                            id: cardBox
                            y: nodeItem.ny - 30
                            x: nodeItem.isRight ? (nodeItem.nx - 30 - 24 - nodeItem.cardW) : (nodeItem.nx + 30 + 24)
                            width: nodeItem.cardW
                            height: 60
                            radius: Theme.radiusMd
                            color: modelData.isLocked ? (Theme.isDark ? "#232629" : "#f5f6f7") : (nodeItem.isHovered ? Theme.bgCardHover : (Theme.isDark ? "#2a2e32" : "#ffffff"))
                            border.width: nodeItem.isHovered ? 2 : 1
                            border.color: nodeItem.isHovered ? Theme.accent : (modelData.isCompleted ? Theme.success : Theme.borderStrong)
                            scale: nodeItem.isHovered ? 1.02 : 1.0

                            Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                            Behavior on border.color { ColorAnimation { duration: 150 } }

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 2

                                RowLayout {
                                    Layout.fillWidth: true

                                    Text {
                                        text: "LESSON " + modelData.index
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 10
                                        font.weight: Font.Black
                                        color: modelData.isCompleted ? Theme.success : (nodeItem.isCurrent ? Theme.accent : Theme.textMuted)
                                    }

                                    Item { Layout.fillWidth: true }

                                    // Status Badge
                                    Rectangle {
                                        Layout.preferredHeight: 18
                                        Layout.preferredWidth: badgeRow.implicitWidth + 12
                                        radius: 9
                                        color: modelData.isCompleted ? Theme.successBg : (nodeItem.isCurrent ? Theme.accentLight : (Theme.isDark ? "#1b1e20" : "#e0e2e4"))

                                        RowLayout {
                                            id: badgeRow
                                            anchors.centerIn: parent
                                            spacing: 4

                                            ForgeLockIcon {
                                                size: 10
                                                color: Theme.textMuted
                                                holeColor: Theme.isDark ? "#1b1e20" : "#e0e2e4"
                                                visible: modelData.isLocked
                                            }

                                            Text {
                                                text: modelData.isCompleted ? "✓ Completed" : (nodeItem.isCurrent ? "Start ▶" : "Locked")
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 10
                                                font.weight: Font.Bold
                                                color: modelData.isCompleted ? Theme.success : (nodeItem.isCurrent ? Theme.accent : Theme.textMuted)
                                            }
                                        }
                                    }
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.title
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                    font.weight: Font.Bold
                                    color: modelData.isLocked ? Theme.textMuted : Theme.textPrimary
                                    elide: Text.ElideRight
                                }
                            }

                            MouseArea {
                                id: cardMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                enabled: !modelData.isLocked
                                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                onClicked: {
                                    if (!modelData.isLocked) root.lessonSelected(modelData.id)
                                }
                            }
                        }

                        // 3. Circular Node with 3D Bevel & Glow
                        Item {
                            id: circleItem
                            x: nodeItem.nx - root.nodeRadius
                            y: nodeItem.ny - root.nodeRadius
                            width: root.nodeRadius * 2
                            height: root.nodeRadius * 2

                            // Pulsing Ring for current active lesson
                            Rectangle {
                                anchors.centerIn: parent
                                width: parent.width + 18
                                height: parent.height + 18
                                radius: width / 2
                                color: "transparent"
                                border.width: 2
                                border.color: Theme.accent
                                visible: nodeItem.isCurrent

                                SequentialAnimation on scale {
                                    loops: Animation.Infinite
                                    running: nodeItem.isCurrent
                                    NumberAnimation { from: 0.95; to: 1.25; duration: 1100; easing.type: Easing.OutQuad }
                                    NumberAnimation { from: 1.25; to: 0.95; duration: 1100; easing.type: Easing.InQuad }
                                }
                                SequentialAnimation on opacity {
                                    loops: Animation.Infinite
                                    running: nodeItem.isCurrent
                                    NumberAnimation { from: 0.8; to: 0.1; duration: 1100; easing.type: Easing.OutQuad }
                                    NumberAnimation { from: 0.1; to: 0.8; duration: 1100; easing.type: Easing.InQuad }
                                }
                            }

                            // 3D Bottom shadow/depth circle
                            Rectangle {
                                anchors.fill: parent
                                anchors.topMargin: 4
                                radius: root.nodeRadius
                                color: modelData.isCompleted ? "#1e824c" : (nodeItem.isCurrent ? "#157cc4" : (Theme.isDark ? "#1b1e20" : "#bdc3c7"))
                            }

                            // Main Stepper Circle
                            Rectangle {
                                id: mainCircle
                                anchors.fill: parent
                                radius: root.nodeRadius
                                color: modelData.isCompleted ? Theme.success : (nodeItem.isCurrent ? Theme.accent : (Theme.isDark ? "#31363b" : "#e0e2e4"))
                                border.width: nodeItem.isHovered ? 3 : 2
                                border.color: nodeItem.isHovered ? "#ffffff" : (modelData.isCompleted ? "#4cd137" : (nodeItem.isCurrent ? "#56baf2" : Theme.borderSubtle))
                                scale: nodeItem.isHovered ? 1.08 : 1.0

                                Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

                                // Center Icon / Status
                                ForgeLockIcon {
                                    anchors.centerIn: parent
                                    size: 22
                                    color: Theme.isDark ? "#8f99a3" : "#69717d"
                                    holeColor: Theme.isDark ? "#31363b" : "#e0e2e4"
                                    visible: modelData.isLocked
                                }

                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.isCompleted ? "✓" : "▶"
                                    font.pixelSize: modelData.isCompleted ? 20 : 16
                                    font.weight: Font.Black
                                    color: "#ffffff"
                                    visible: !modelData.isLocked
                                }
                            }

                            // Mini lesson number badge pinned below
                            Rectangle {
                                anchors.top: mainCircle.bottom
                                anchors.topMargin: 4
                                anchors.horizontalCenter: mainCircle.horizontalCenter
                                width: numLbl.implicitWidth + 8
                                height: 16
                                radius: 8
                                color: Theme.isDark ? "#232629" : "#eef0f2"
                                border.width: 1
                                border.color: Theme.borderSubtle

                                Text {
                                    id: numLbl
                                    anchors.centerIn: parent
                                    text: "№" + modelData.index
                                    font.family: Theme.fontCode
                                    font.pixelSize: 10
                                    font.weight: Font.Bold
                                    color: Theme.textSecondary
                                }
                            }

                            MouseArea {
                                id: circleMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                enabled: !modelData.isLocked
                                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                onClicked: {
                                    if (!modelData.isLocked) root.lessonSelected(modelData.id)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
