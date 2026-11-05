import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic
import CppForge

Window {
    id: authWin

    visible: false
    width: 1024
    height: 720
    minimumWidth: 640
    minimumHeight: 520

    title: "CppForge"
    flags: Qt.Window | Qt.FramelessWindowHint
    color: Theme.bgApp

    // Passed from C++ or instantiated
    property var authController

    signal switchToMainMenu(string username, int userId)

    Connections {
        target: authWin.authController
        function onLoginSucceeded(username, userId) {
            authWin.switchToMainMenu(username, userId)
        }
    }

    Column {
        anchors.fill: parent

        ForgeTitleBar {
            id: titleBar
            width: parent.width
            title: isSignUp ? "CppForge — Create Account" : "CppForge — Log In"
            targetWindow: authWin
        }

        Item {
            width: parent.width
            height: parent.height - titleBar.height

            // Decorative background cards & watermark icons from stat cards (Solved Tasks, Lessons, Duels, Winrate)
            AuthBackgroundDecorations {
                anchors.fill: parent
            }

            // Centered Scrollable Container
            Flickable {
                id: flick
                anchors.fill: parent
                contentWidth: width
                contentHeight: Math.max(height, viewCard.height + Theme.spacingXxl * 2)
                clip: true

                // Elevated Breeze Card Surface for Auth Form
                Rectangle {
                    id: viewCard
                    width: 488
                    height: (isSignUp ? signUpView.implicitHeight : authView.implicitHeight) + 48
                    anchors.centerIn: parent
                    radius: Theme.radiusLg
                    color: Theme.bgCard
                    border.width: 1
                    border.color: Theme.borderSubtle

                    Item {
                        id: viewContainer
                        width: 440
                        height: isSignUp ? signUpView.implicitHeight : authView.implicitHeight
                        anchors.centerIn: parent

                        AuthView {
                            id: authView
                            anchors.fill: parent
                            authController: authWin.authController
                            visible: opacity > 0
                            opacity: isSignUp ? 0.0 : 1.0

                            Behavior on opacity {
                                NumberAnimation { duration: Theme.animNormal }
                            }

                            onSwitchToSignUp: {
                                if (authWin.authController) authWin.authController.clearMessages()
                                isSignUp = true
                            }
                        }

                        SignUpView {
                            id: signUpView
                            anchors.fill: parent
                            authController: authWin.authController
                            visible: opacity > 0
                            opacity: isSignUp ? 1.0 : 0.0

                            Behavior on opacity {
                                NumberAnimation { duration: Theme.animNormal }
                            }

                            onSwitchToLogin: {
                                if (authWin.authController) authWin.authController.clearMessages()
                                isSignUp = false
                            }
                        }
                    }
                }
            }
        }
    }

    property bool isSignUp: false
}
