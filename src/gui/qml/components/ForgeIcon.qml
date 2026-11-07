import QtQuick
import CppForge

Item {
    id: control

    property string name: "code"
    property color color: Theme.textPrimary
    property int size: 16

    implicitWidth: size
    implicitHeight: size

    onNameChanged: canvas.requestPaint()
    onColorChanged: canvas.requestPaint()
    onSizeChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            var w = width;
            var h = height;
            var c = control.color.toString();

            ctx.save();
            ctx.strokeStyle = c;
            ctx.fillStyle = c;
            ctx.lineWidth = Math.max(1.5, w / 11);
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            switch (control.name) {
                case "learn":
                case "book":
                    // Open book icon
                    var midX = w / 2;
                    var bY = h * 0.22;
                    var eY = h * 0.78;
                    // Left page
                    ctx.beginPath();
                    ctx.moveTo(midX, bY + 3);
                    ctx.bezierCurveTo(midX - w * 0.2, bY - 2, midX - w * 0.4, bY, w * 0.1, bY + 2);
                    ctx.lineTo(w * 0.1, eY + 2);
                    ctx.bezierCurveTo(midX - w * 0.4, eY, midX - w * 0.2, eY - 2, midX, eY + 3);
                    ctx.stroke();
                    // Right page
                    ctx.beginPath();
                    ctx.moveTo(midX, bY + 3);
                    ctx.bezierCurveTo(midX + w * 0.2, bY - 2, midX + w * 0.4, bY, w * 0.9, bY + 2);
                    ctx.lineTo(w * 0.9, eY + 2);
                    ctx.bezierCurveTo(midX + w * 0.4, eY, midX + w * 0.2, eY - 2, midX, eY + 3);
                    ctx.stroke();
                    // Spine
                    ctx.beginPath();
                    ctx.moveTo(midX, bY + 3);
                    ctx.lineTo(midX, eY + 3);
                    ctx.stroke();
                    break;

                case "duels":
                case "swords":
                    // Crossed blades
                    ctx.beginPath();
                    ctx.moveTo(w * 0.2, h * 0.2);
                    ctx.lineTo(w * 0.8, h * 0.8);
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.moveTo(w * 0.8, h * 0.2);
                    ctx.lineTo(w * 0.2, h * 0.8);
                    ctx.stroke();
                    // Guards
                    ctx.beginPath();
                    ctx.moveTo(w * 0.68, h * 0.78);
                    ctx.lineTo(w * 0.78, h * 0.68);
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.moveTo(w * 0.22, h * 0.68);
                    ctx.lineTo(w * 0.32, h * 0.78);
                    ctx.stroke();
                    break;

                case "profile":
                case "user":
                    // Head circle
                    var headR = w * 0.18;
                    ctx.beginPath();
                    ctx.arc(w / 2, h * 0.32, headR, 0, Math.PI * 2);
                    ctx.stroke();
                    // Shoulders arc
                    ctx.beginPath();
                    ctx.arc(w / 2, h * 0.95, w * 0.38, Math.PI * 1.15, Math.PI * 1.85);
                    ctx.stroke();
                    break;

                case "logout":
                    // Door frame
                    ctx.beginPath();
                    ctx.moveTo(w * 0.55, h * 0.15);
                    ctx.lineTo(w * 0.2, h * 0.15);
                    ctx.lineTo(w * 0.2, h * 0.85);
                    ctx.lineTo(w * 0.55, h * 0.85);
                    ctx.stroke();
                    // Arrow pointing right
                    ctx.beginPath();
                    ctx.moveTo(w * 0.4, h * 0.5);
                    ctx.lineTo(w * 0.85, h * 0.5);
                    ctx.lineTo(w * 0.65, h * 0.35);
                    ctx.moveTo(w * 0.85, h * 0.5);
                    ctx.lineTo(w * 0.65, h * 0.65);
                    ctx.stroke();
                    break;

                case "search":
                    // Magnifying glass
                    var glassR = w * 0.26;
                    var gX = w * 0.42;
                    var gY = h * 0.42;
                    ctx.beginPath();
                    ctx.arc(gX, gY, glassR, 0, Math.PI * 2);
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.moveTo(gX + glassR * 0.7, gY + glassR * 0.7);
                    ctx.lineTo(w * 0.85, h * 0.85);
                    ctx.lineWidth = Math.max(2, w / 9);
                    ctx.stroke();
                    break;

                case "bulb":
                case "tip":
                case "idea":
                    // Lightbulb
                    ctx.beginPath();
                    ctx.arc(w / 2, h * 0.38, w * 0.24, Math.PI * 0.8, Math.PI * 2.2);
                    ctx.lineTo(w * 0.58, h * 0.72);
                    ctx.lineTo(w * 0.42, h * 0.72);
                    ctx.closePath();
                    ctx.stroke();
                    // Base lines
                    ctx.beginPath();
                    ctx.moveTo(w * 0.44, h * 0.82);
                    ctx.lineTo(w * 0.56, h * 0.82);
                    ctx.moveTo(w * 0.47, h * 0.9);
                    ctx.lineTo(w * 0.53, h * 0.9);
                    ctx.stroke();
                    break;

                case "target":
                case "goal":
                    // Concentric rings
                    ctx.beginPath();
                    ctx.arc(w / 2, h / 2, w * 0.42, 0, Math.PI * 2);
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.arc(w / 2, h / 2, w * 0.25, 0, Math.PI * 2);
                    ctx.stroke();
                    // Center dot
                    ctx.beginPath();
                    ctx.arc(w / 2, h / 2, Math.max(1.8, w * 0.08), 0, Math.PI * 2);
                    ctx.fill();
                    break;

                case "trophy":
                case "cup":
                    // Cup body
                    ctx.beginPath();
                    ctx.moveTo(w * 0.26, h * 0.2);
                    ctx.lineTo(w * 0.74, h * 0.2);
                    ctx.lineTo(w * 0.68, h * 0.52);
                    ctx.bezierCurveTo(w * 0.65, h * 0.68, w * 0.35, h * 0.68, w * 0.32, h * 0.52);
                    ctx.closePath();
                    ctx.stroke();
                    // Stem & Base
                    ctx.beginPath();
                    ctx.moveTo(w / 2, h * 0.65);
                    ctx.lineTo(w / 2, h * 0.82);
                    ctx.moveTo(w * 0.32, h * 0.85);
                    ctx.lineTo(w * 0.68, h * 0.85);
                    ctx.stroke();
                    // Handles
                    ctx.beginPath();
                    ctx.arc(w * 0.22, h * 0.36, w * 0.1, -Math.PI * 0.5, Math.PI * 0.5, true);
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.arc(w * 0.78, h * 0.36, w * 0.1, -Math.PI * 0.5, Math.PI * 0.5, false);
                    ctx.stroke();
                    break;

                case "fire":
                case "streak":
                    // Flame contour
                    ctx.beginPath();
                    ctx.moveTo(w / 2, h * 0.12);
                    ctx.bezierCurveTo(w * 0.72, h * 0.3, w * 0.85, h * 0.6, w * 0.72, h * 0.82);
                    ctx.bezierCurveTo(w * 0.6, h * 0.94, w * 0.4, h * 0.94, w * 0.28, h * 0.82);
                    ctx.bezierCurveTo(w * 0.15, h * 0.6, w * 0.28, h * 0.3, w / 2, h * 0.12);
                    ctx.stroke();
                    // Inner mini flame
                    ctx.beginPath();
                    ctx.moveTo(w / 2, h * 0.52);
                    ctx.bezierCurveTo(w * 0.62, h * 0.65, w * 0.6, h * 0.78, w / 2, h * 0.82);
                    ctx.bezierCurveTo(w * 0.4, h * 0.78, w * 0.38, h * 0.65, w / 2, h * 0.52);
                    ctx.fill();
                    break;

                case "code":
                    // < / > brackets
                    // Left <
                    ctx.beginPath();
                    ctx.moveTo(w * 0.36, h * 0.28);
                    ctx.lineTo(w * 0.16, h * 0.5);
                    ctx.lineTo(w * 0.36, h * 0.72);
                    ctx.stroke();
                    // Right >
                    ctx.beginPath();
                    ctx.moveTo(w * 0.64, h * 0.28);
                    ctx.lineTo(w * 0.84, h * 0.5);
                    ctx.lineTo(w * 0.64, h * 0.72);
                    ctx.stroke();
                    // Slash /
                    ctx.beginPath();
                    ctx.moveTo(w * 0.56, h * 0.22);
                    ctx.lineTo(w * 0.44, h * 0.78);
                    ctx.stroke();
                    break;

                case "check":
                    // Checkmark
                    ctx.beginPath();
                    ctx.moveTo(w * 0.18, h * 0.52);
                    ctx.lineTo(w * 0.42, h * 0.76);
                    ctx.lineTo(w * 0.84, h * 0.26);
                    ctx.lineWidth = Math.max(2, w / 8);
                    ctx.stroke();
                    break;

                case "lock":
                    // Padlock
                    var sW = w * 0.48;
                    var sL = (w - sW) / 2;
                    var sR = sW / 2;
                    var sT = h * 0.12;
                    var bT = h * 0.44;
                    ctx.beginPath();
                    ctx.moveTo(sL, bT + 2);
                    ctx.lineTo(sL, sT + sR);
                    ctx.arc(w / 2, sT + sR, sR, Math.PI, 0, false);
                    ctx.lineTo(sL + sW, bT + 2);
                    ctx.stroke();
                    // Body
                    var bW = w * 0.74;
                    var bH = h * 0.50;
                    var bX = (w - bW) / 2;
                    var bY = bT;
                    var cr = Math.max(2, w * 0.12);
                    ctx.beginPath();
                    ctx.moveTo(bX + cr, bY);
                    ctx.lineTo(bX + bW - cr, bY);
                    ctx.quadraticCurveTo(bX + bW, bY, bX + bW, bY + cr);
                    ctx.lineTo(bX + bW, bY + bH - cr);
                    ctx.quadraticCurveTo(bX + bW, bY + bH, bX + bW - cr, bY + bH);
                    ctx.lineTo(bX + cr, bY + bH);
                    ctx.quadraticCurveTo(bX, bY + bH, bX, bY + bH - cr);
                    ctx.lineTo(bX, bY + cr);
                    ctx.quadraticCurveTo(bX, bY, bX + cr, bY);
                    ctx.closePath();
                    ctx.fill();
                    break;

                case "gear":
                case "settings":
                    // Gear cog
                    ctx.beginPath();
                    ctx.arc(w / 2, h / 2, w * 0.2, 0, Math.PI * 2);
                    ctx.stroke();
                    var teeth = 6;
                    for (var i = 0; i < teeth; i++) {
                        var angle = (i * Math.PI * 2) / teeth;
                        var x1 = w / 2 + Math.cos(angle) * (w * 0.26);
                        var y1 = h / 2 + Math.sin(angle) * (h * 0.26);
                        var x2 = w / 2 + Math.cos(angle) * (w * 0.42);
                        var y2 = h / 2 + Math.sin(angle) * (h * 0.42);
                        ctx.beginPath();
                        ctx.moveTo(x1, y1);
                        ctx.lineTo(x2, y2);
                        ctx.stroke();
                    }
                    break;

                case "sun":
                    // Sun core
                    ctx.beginPath();
                    ctx.arc(w / 2, h / 2, w * 0.22, 0, Math.PI * 2);
                    ctx.stroke();
                    // Sun rays
                    var rays = 8;
                    for (var r = 0; r < rays; r++) {
                        var ra = (r * Math.PI * 2) / rays;
                        var rx1 = w / 2 + Math.cos(ra) * (w * 0.32);
                        var ry1 = h / 2 + Math.sin(ra) * (h * 0.32);
                        var rx2 = w / 2 + Math.cos(ra) * (w * 0.44);
                        var ry2 = h / 2 + Math.sin(ra) * (h * 0.44);
                        ctx.beginPath();
                        ctx.moveTo(rx1, ry1);
                        ctx.lineTo(rx2, ry2);
                        ctx.stroke();
                    }
                    break;

                case "moon":
                    // Crescent moon
                    ctx.beginPath();
                    ctx.arc(w * 0.48, h / 2, w * 0.36, -Math.PI * 0.4, Math.PI * 0.4, false);
                    ctx.arc(w * 0.34, h / 2, w * 0.28, Math.PI * 0.4, -Math.PI * 0.4, true);
                    ctx.closePath();
                    ctx.fill();
                    break;

                default:
                    // Generic bullet dot
                    ctx.beginPath();
                    ctx.arc(w / 2, h / 2, w * 0.3, 0, Math.PI * 2);
                    ctx.fill();
                    break;
            }

            ctx.restore();
        }
    }
}
