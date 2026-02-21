#pragma once

#include "SignUpWindow.hpp"
#include "CustomTitleBar.hpp"
#include "../../core/include/services/AuthManager.hpp"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>

#include <memory>

class AuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent = nullptr);
    ~AuthWindow();

    void fadeIn();

private:
    void setupUI();
    void setupTitleBar();
    void setupLogo();
    void setupTitle();
    void setupInputFields();
    void setupWindowProperties();
    void setupLoginButton();
    void setupCreateAccountLink();
    void setupForgotPasswordLink();
    void setupLayout();
    void showFallbackLogo();
    void centerWindow();
    void togglePasswordVisibility();

    void openSignUpWindow();
    void openForgotPasswordWindow();
    
    void onLoginClicked();
    void onCreateAccountClicked();
    void onForgotPasswordClicked();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
    std::unique_ptr<QLabel> iconLabel_;
    std::unique_ptr<QLabel> titleLabel_;
    std::unique_ptr<CustomTitleBar> customTitleBar_;
    
    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> passwordInput_;
    
    std::unique_ptr<QPushButton> loginButton_;
    std::unique_ptr<QPushButton> createAccountButton_;
    std::unique_ptr<QPushButton> forgotPasswordButton_;
    std::unique_ptr<QPushButton> passwordToggleButton_;

    std::unique_ptr<SignUpWindow> signUpWindow_;
    
    std::unique_ptr<QVBoxLayout> mainLayout_;
    
    std::shared_ptr<cppforge::services::AuthManager> authManager_;
    bool passwordVisible_ = false;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
};