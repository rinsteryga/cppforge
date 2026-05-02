#pragma once

#include "../../core/include/services/AuthManager.hpp"
#include "../../core/include/services/ThemeService.hpp"

#include <QFont>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

class CustomTitleBar;

class SignUpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SignUpWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent = nullptr);
    ~SignUpWindow();

    void fadeIn();
    void setThemeService(cppforge::services::ThemeService *service);

signals:
    void switchToLogin();

private:
    void onSignUpButtonClicked();
    void onBackToLoginClicked();
    void togglePasswordVisibility();

protected:
    void paintEvent(QPaintEvent *event) override;

    void setupUI();
    void setupTitleBar();
    void setupWindowProperties();
    void centerWindow();

    void setupLogo();
    void setupTitle();
    void setupInputFields();
    void setupSignUpButton();
    void setupBackToLoginLink();
    void setupLayout();
    void setupStyles();
    void setupConnections();

    void showFallbackLogo();

    std::unique_ptr<QLabel> iconLabel_;
    std::unique_ptr<QLabel> titleLabel_;
    std::unique_ptr<CustomTitleBar> customTitleBar_;

    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> emailInput_;
    std::unique_ptr<QLineEdit> passwordInput_;

    std::unique_ptr<QPushButton> signUpButton_;
    std::unique_ptr<QPushButton> backToLoginButton_;
    std::unique_ptr<QPushButton> passwordToggleButton_;

    std::unique_ptr<QVBoxLayout> mainLayout_;

    cppforge::services::ThemeService *themeService_ = nullptr;
    std::shared_ptr<cppforge::services::AuthManager> authManager_;
    bool passwordVisible_ = false;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
};