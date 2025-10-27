#pragma once

#include "../include/signupwindow.hpp"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QVBoxLayout>

#include <memory>

class AuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);

private:
    void onSignInButtonClicked();
    void onCloseButtonClicked();
    void onSignUpButtonClicked();

    void setupUI();

    void setupWindowProperties();
    void setupLabelFont(QLabel *label_);
    void setupMainLabel();
    void setupUsernameLabel();
    void setupPasswordLabel();
    void setupButtonFont(QPushButton *button_);
    void setupSignUpLabel();
    void setupSignUpButton();
    void setupLogInButton();
    void setupEditFields();
    void setupEditFieldFont(QLineEdit *edit_);
    void setupCloseButton();
    void setupLayout();

    void openSignUpWindow();

    void setupConnections();

    std::unique_ptr<QLabel> mainLabel_;
    std::unique_ptr<QLabel> usernameLabel_;
    std::unique_ptr<QLabel> passwordLabel_;
    std::unique_ptr<QLabel> signUpLabel_;

    std::unique_ptr<QFont> labelFont_;
    std::unique_ptr<QFont> editFieldFont_;
    std::unique_ptr<QFont> loginButtonFont_;

    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> passwordInput_;
    
    std::unique_ptr<QPushButton> signUpButton_;
    std::unique_ptr<QPushButton> logInButton_;
    std::unique_ptr<QPushButton> closeButton_;

    std::unique_ptr<QVBoxLayout> mainLayout_;

    std::unique_ptr<SignUpWindow> signUpWindow_;
};
