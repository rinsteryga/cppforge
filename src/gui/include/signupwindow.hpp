#pragma once

#include <Qwidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>

#include <memory>

class SignUpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SignUpWindow(QWidget *parent = nullptr);

private:
    void onSignUpButtonClicked();
    void onCloseButtonClicked();

    void setupUI();

    void setupWindowProperties();

    void setupLabelFont(QLabel *label_);
    void setupEditFieldFont(QLineEdit *editField_);
    void setupButtonFont(QPushButton *button_);

    void setupMainLabel();
    void setupUsernameLabel();
    void setupEmailLabel();
    void setupPasswordLabel();

    void setupEditFields();

    void setupSignUpButton();
    void setupCloseButton();

    void setupLayout();

    void setupConnections();

    std::unique_ptr<QLabel> mainLabel_;
    std::unique_ptr<QLabel> usernameLabel_;
    std::unique_ptr<QLabel> emailLabel_;
    std::unique_ptr<QLabel> passwordLabel_;

    std::unique_ptr<QFont> labelFont_;
    std::unique_ptr<QFont> buttonFont_;
    std::unique_ptr<QFont> editFieldFont_;

    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> emailInput_;
    std::unique_ptr<QLineEdit> passwordInput_;

    std::unique_ptr<QPushButton> signUpButton_;
    std::unique_ptr<QPushButton> closeButton_;

    std::unique_ptr<QVBoxLayout> mainLayout_;
};
