import QtQuick
import CppForge

Rectangle {
    id: control

    property var model: ["Tab 1", "Tab 2"]
    property int currentIndex: 0
    signal indexSelected(int index)

    implicitHeight: 32
    implicitWidth: segRow.implicitWidth + 4
    radius: Theme.radiusMd
    color: Theme.bgInput
    border.width: 1
    border.color: Theme.borderSubtle

    // Sliding indicator behind the active tab
    Rectangle {
        id: indicator
        height: control.height - 4
        y: 2
        radius: Theme.radiusSm
        color: Theme.bgCard
        border.width: 1
        border.color: Theme.borderStrong

        // Animate position and width
        Behavior on x {
            NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
        }
        Behavior on width {
            NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
        }
    }

    Row {
        id: segRow
        anchors.fill: parent
        anchors.margins: 2

        Repeater {
            id: rep
            model: control.model

            Item {
                id: tabItem
                height: segRow.height
                width: Math.max(tabText.implicitWidth + 24, 70)

                Text {
                    id: tabText
                    anchors.centerIn: parent
                    text: modelData
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeSm
                    font.weight: control.currentIndex === index ? Font.Bold : Font.Normal
                    color: control.currentIndex === index ? Theme.accent : (tabMouse.containsMouse ? Theme.textPrimary : Theme.textSecondary)

                    Behavior on color { ColorAnimation { duration: Theme.animFast } }
                }

                MouseArea {
                    id: tabMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        control.currentIndex = index
                        control.indexSelected(index)
                    }
                }
            }
        }
    }

    // Update indicator frame whenever currentIndex or layout changes
    function updateIndicator() {
        if (rep.count > control.currentIndex && control.currentIndex >= 0) {
            var item = rep.itemAt(control.currentIndex)
            if (item) {
                indicator.x = item.x + 2
                indicator.width = item.width
            }
        }
    }

    onCurrentIndexChanged: updateIndicator()
    Component.onCompleted: updateIndicator()
    onWidthChanged: Qt.callLater(updateIndicator)
}
