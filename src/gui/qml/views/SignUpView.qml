import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var authController

    signal switchToLogin()

    implicitWidth: 440
    implicitHeight: mainCol.implicitHeight

    ColumnLayout {
        id: mainCol
        anchors.fill: parent
        spacing: 20

        // 1. Logo (changes with theme)
        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 80
            Layout.preferredHeight: 80
            source: Theme.logoPath
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        // 2. Title strictly centered
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            text: "Create your account"
            font.family: Theme.fontFamily
            font.pixelSize: 26
            font.weight: Font.Bold
            color: Theme.textPrimary
        }

        Item {
            Layout.preferredHeight: 5
        }

        // Error message banner
        Rectangle {
            id: errorBanner
            Layout.fillWidth: true
            Layout.preferredHeight: errText.implicitHeight + Theme.spacingMd * 2
            visible: authController && authController.errorMessage !== ""
            radius: Theme.radiusMd
            color: Theme.dangerBg
            border.color: Theme.danger
            border.width: 1

            Text {
                id: errText
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                text: authController ? authController.errorMessage : ""
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSm
                color: Theme.danger
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        // 3. Username Input
        ForgeTextField {
            id: usernameField
            Layout.fillWidth: true
            placeholderText: "Username (3-20 characters)"
            onAccepted: emailField.inputFocus = true
        }

        // 4. Email Input
        ForgeTextField {
            id: emailField
            Layout.fillWidth: true
            placeholderText: "Email address"
            onAccepted: passwordField.inputFocus = true
        }

        // 5. Password Input
        ForgeTextField {
            id: passwordField
            Layout.fillWidth: true
            placeholderText: "Password (min. 6 characters)"
            isPassword: true
            onAccepted: registerBtn.clicked()
        }

        Item {
            Layout.preferredHeight: 10
        }

        // 6. Create Account Button (KDE Breeze style)
        ForgeButton {
            id: registerBtn
            Layout.fillWidth: true
            implicitHeight: 40
            text: "Create Account"
            font.pixelSize: 15
            font.weight: Font.DemiBold
            variant: "primary"
            loading: authController ? authController.isLoading : false
            onClicked: {
                if (authController) {
                    if (authController.registerUser(usernameField.text, emailField.text, passwordField.text)) {
                        root.switchToLogin()
                    }
                }
            }
        }

        // 7. Back to Login link
        MouseArea {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 36
            Layout.preferredWidth: linkText.implicitWidth + 20
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.switchToLogin()

            Text {
                id: linkText
                anchors.centerIn: parent
                text: "Already have an account? Sign in"
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.underline: parent.containsMouse
                color: parent.containsMouse ? Theme.accentHover : Theme.linkColor

                Behavior on color {
                    ColorAnimation { duration: Theme.animFast }
                }
            }
        }
    }
}
