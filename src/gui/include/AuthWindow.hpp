#pragma once

#include "SignUpWindow.hpp"

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <memory>


class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupLogo();
    void setupTitle();
    void setupInputFields();
    void setupWindowProperties();
    void setupLoginButton();
    void setupCreateAccountLink();
    void setupLayout();
    void showFallbackLogo();
    void centerWindow();
    void togglePasswordVisibility();

    void openSignUpWindow();
    
    void onLoginClicked();
    void onCreateAccountClicked();
    
    std::unique_ptr<QWidget> centralWidget_;
    
    std::unique_ptr<QLabel> iconLabel_;
    std::unique_ptr<QLabel> titleLabel_;
    
    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> passwordInput_;
    
    std::unique_ptr<QPushButton> loginButton_;
    std::unique_ptr<QPushButton> createAccountButton_;
    std::unique_ptr<QPushButton> passwordToggleButton_;

    std::unique_ptr<SignUpWindow> signUpWindow_;
    
    std::unique_ptr<QVBoxLayout> mainLayout_;
    
    bool passwordVisible_ = false;
};