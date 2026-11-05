import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import CppForge

Item {
    id: root

    property var authController

    signal switchToSignUp()

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
            text: "Log in to cppforge"
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

        // Success message banner
        Rectangle {
            id: successBanner
            Layout.fillWidth: true
            Layout.preferredHeight: succText.implicitHeight + Theme.spacingMd * 2
            visible: authController && authController.successMessage !== ""
            radius: Theme.radiusMd
            color: Theme.successBg
            border.color: Theme.success
            border.width: 1

            Text {
                id: succText
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                text: authController ? authController.successMessage : ""
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeSm
                color: Theme.success
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        // 3. Username Input
        ForgeTextField {
            id: usernameField
            Layout.fillWidth: true
            placeholderText: "Username or email address"
            onAccepted: passwordField.inputFocus = true
        }

        // 4. Password Input
        ForgeTextField {
            id: passwordField
            Layout.fillWidth: true
            placeholderText: "Password"
            isPassword: true
            onAccepted: loginBtn.clicked()
        }

        // 5. Remember me checkbox
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingSm

            CheckBox {
                id: rememberCheck
                checked: true

                indicator: Rectangle {
                    implicitWidth: 18
                    implicitHeight: 18
                    radius: Theme.radiusXs
                    color: rememberCheck.checked ? Theme.accent : Theme.bgInput
                    border.color: rememberCheck.checked ? Theme.accent : Theme.borderSubtle
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "✓"
                        font.pixelSize: 12
                        color: Theme.textOnAccent
                        visible: rememberCheck.checked
                    }
                }

                contentItem: Text {
                    text: "Remember me"
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeSm
                    color: Theme.textSecondary
                    leftPadding: rememberCheck.indicator.width + Theme.spacingSm
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Item {
            Layout.preferredHeight: 10
        }

        // 6. Login Button (KDE Breeze style)
        ForgeButton {
            id: loginBtn
            Layout.fillWidth: true
            implicitHeight: 40
            text: "Log in"
            font.pixelSize: 15
            font.weight: Font.DemiBold
            variant: "primary"
            loading: authController ? authController.isLoading : false
            onClicked: {
                if (authController) {
                    authController.login(usernameField.text, passwordField.text, rememberCheck.checked)
                }
            }
        }

        // 7. Create account link
        MouseArea {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 36
            Layout.preferredWidth: linkText.implicitWidth + 20
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.switchToSignUp()

            Text {
                id: linkText
                anchors.centerIn: parent
                text: "New to cppforge? Create an account"
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
