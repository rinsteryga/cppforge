import QtQuick
import CppForge

Item {
    id: control

    property color color: Theme.textMuted
    property int size: 16
    property color holeColor: Theme.isDark ? "#31363b" : "#ffffff"

    implicitWidth: size
    implicitHeight: size

    onColorChanged: canvas.requestPaint()
    onSizeChanged: canvas.requestPaint()
    onHoleColorChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.clearRect(0, 0, width, height)

            var w = width
            var h = height

            ctx.save()

            // 1. Shackle (Upper U-arch)
            var shackleW = w * 0.48
            var shackleLeft = (w - shackleW) / 2
            var shackleRadius = shackleW / 2
            var shackleTop = h * 0.12
            var bodyTop = h * 0.44
            var shackleLineWidth = Math.max(1.8, w * 0.13)

            ctx.strokeStyle = control.color.toString()
            ctx.lineWidth = shackleLineWidth
            ctx.lineCap = "round"
            ctx.lineJoin = "round"

            ctx.beginPath()
            ctx.moveTo(shackleLeft, bodyTop + 2)
            ctx.lineTo(shackleLeft, shackleTop + shackleRadius)
            ctx.arc(w / 2, shackleTop + shackleRadius, shackleRadius, Math.PI, 0, false)
            ctx.lineTo(shackleLeft + shackleW, bodyTop + 2)
            ctx.stroke()

            // 2. Lock Body (Rounded Rectangle)
            var bodyW = w * 0.74
            var bodyH = h * 0.50
            var bodyX = (w - bodyW) / 2
            var bodyY = bodyTop
            var r = Math.max(2, w * 0.12)

            ctx.fillStyle = control.color.toString()
            ctx.beginPath()
            ctx.moveTo(bodyX + r, bodyY)
            ctx.lineTo(bodyX + bodyW - r, bodyY)
            ctx.quadraticCurveTo(bodyX + bodyW, bodyY, bodyX + bodyW, bodyY + r)
            ctx.lineTo(bodyX + bodyW, bodyY + bodyH - r)
            ctx.quadraticCurveTo(bodyX + bodyW, bodyY + bodyH, bodyX + bodyW - r, bodyY + bodyH)
            ctx.lineTo(bodyX + r, bodyY + bodyH)
            ctx.quadraticCurveTo(bodyX, bodyY + bodyH, bodyX, bodyY + bodyH - r)
            ctx.lineTo(bodyX, bodyY + r)
            ctx.quadraticCurveTo(bodyX, bodyY, bodyX + r, bodyY)
            ctx.closePath()
            ctx.fill()

            // 3. Keyhole (Circle + Slot cutout)
            ctx.fillStyle = control.holeColor.toString()
            var holeCenterX = w / 2
            var holeCenterY = bodyY + bodyH * 0.40
            var holeRadius = Math.max(1.2, w * 0.08)

            ctx.beginPath()
            ctx.arc(holeCenterX, holeCenterY, holeRadius, 0, Math.PI * 2)
            ctx.fill()

            ctx.beginPath()
            var slotTopW = holeRadius * 0.7
            var slotBotW = holeRadius * 0.9
            var slotBotY = bodyY + bodyH * 0.76
            ctx.moveTo(holeCenterX - slotTopW, holeCenterY)
            ctx.lineTo(holeCenterX + slotTopW, holeCenterY)
            ctx.lineTo(holeCenterX + slotBotW, slotBotY)
            ctx.lineTo(holeCenterX - slotBotW, slotBotY)
            ctx.closePath()
            ctx.fill()

            ctx.restore()
        }
    }
}
