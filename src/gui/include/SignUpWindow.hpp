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
    void switchToLogin();  // Сигнал для возврата к окну входа

private slots:
    void onSignUpButtonClicked();
    void onBackToLoginClicked();
    void togglePasswordVisibility();

private:
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
    
    // Виджеты
    std::unique_ptr<QLabel> iconLabel;
    std::unique_ptr<QLabel> titleLabel;
    
    std::unique_ptr<QLineEdit> usernameInput;
    std::unique_ptr<QLineEdit> emailInput;
    std::unique_ptr<QLineEdit> passwordInput;
    
    std::unique_ptr<QPushButton> signUpButton;
    std::unique_ptr<QPushButton> backToLoginButton;
    std::unique_ptr<QPushButton> passwordToggleButton;
    
    std::unique_ptr<QVBoxLayout> mainLayout;
    
    bool passwordVisible = false;
};