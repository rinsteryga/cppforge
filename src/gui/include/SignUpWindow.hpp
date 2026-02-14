#pragma once

#include <QWidget>
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

signals:
    void switchToLogin();

private:
    void onSignUpButtonClicked();
    void onBackToLoginClicked();
    void togglePasswordVisibility();

    void setupUI();
    void setupWindowProperties();
    void centerWindow();
    
    void setupLogo();
    void setupTitle();
    void setupInputFields();
    void setupSignUpButton();
    void setupBackToLoginLink();
    void setupLayout();
    void setupConnections();
    
    void showFallbackLogo();

    std::unique_ptr<QLabel> iconLabel_;
    std::unique_ptr<QLabel> titleLabel_;
    
    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> emailInput_;
    std::unique_ptr<QLineEdit> passwordInput_;
    
    std::unique_ptr<QPushButton> signUpButton_;
    std::unique_ptr<QPushButton> backToLoginButton_;
    std::unique_ptr<QPushButton> passwordToggleButton_;
    
    std::unique_ptr<QVBoxLayout> mainLayout_;
    
    bool passwordVisible_ = false;
};