import QtQuick
import QtQuick.Layouts
import CppForge

Item {
    id: control

    // Value between 0.0 and 1.0 (or custom range)
    property real value: 0.0
    property real minValue: 0.0
    property real maxValue: 100.0

    property string title: "Metric"
    property string valueText: Math.round(control.value) + "%"
    property string detailText: ""
    property color gaugeColor: Theme.accent
    property color trackColor: Theme.borderSubtle
    property int strokeWidth: 10

    // Arc range: default 240 degrees (from 150 deg to 390 deg)
    property real startAngleDeg: 150
    property real sweepAngleDeg: 240

    implicitWidth: 160
    implicitHeight: 180

    // Animated value for smooth transitions
    property real animatedValue: value
    Behavior on animatedValue {
        NumberAnimation { duration: 400; easing.type: Easing.OutCubic }
    }

    onAnimatedValueChanged: canvas.requestPaint()
    onGaugeColorChanged: canvas.requestPaint()
    onTrackColorChanged: canvas.requestPaint()

    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        // Top Title (like "Memory", "Disk", "CPU")
        Text {
            visible: control.title !== ""
            Layout.alignment: Qt.AlignHCenter
            text: control.title
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSizeLg
            font.weight: Font.Bold
            color: Theme.textPrimary
        }

        // Circular Canvas + Center Text
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Canvas {
                id: canvas
                anchors.fill: parent
                antialiasing: true

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    var centerX = width / 2;
                    var centerY = height / 2;
                    var radius = Math.min(centerX, centerY) - control.strokeWidth;
                    if (radius <= 0) return;

                    var startRad = (control.startAngleDeg * Math.PI) / 180.0;
                    var endRad = ((control.startAngleDeg + control.sweepAngleDeg) * Math.PI) / 180.0;

                    // 1. Background Track Arc
                    ctx.beginPath();
                    ctx.arc(centerX, centerY, radius, startRad, endRad, false);
                    ctx.lineWidth = control.strokeWidth;
                    ctx.lineCap = "round";
                    ctx.strokeStyle = control.trackColor.toString();
                    ctx.stroke();

                    // 2. Active Progress Arc
                    var progressFraction = (control.animatedValue - control.minValue) / Math.max(1.0, (control.maxValue - control.minValue));
                    progressFraction = Math.max(0.0, Math.min(1.0, progressFraction));

                    if (progressFraction > 0.001) {
                        var progressEndRad = startRad + (progressFraction * control.sweepAngleDeg * Math.PI) / 180.0;
                        ctx.beginPath();
                        ctx.arc(centerX, centerY, radius, startRad, progressEndRad, false);
                        ctx.lineWidth = control.strokeWidth;
                        ctx.lineCap = "round";
                        ctx.strokeStyle = control.gaugeColor.toString();
                        ctx.stroke();
                    }
                }
            }

            // Center Content: Value and Detail
            Column {
                anchors.centerIn: parent
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: control.valueText
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeXxl
                    font.weight: Font.Bold
                    color: Theme.textPrimary
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: control.detailText
                    visible: control.detailText !== ""
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeXs
                    color: Theme.textMuted
                }
            }
        }
    }
}
