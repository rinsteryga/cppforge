#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>

#include "SignUpWindow.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

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
    
    std::unique_ptr<QWidget> centralWidget;
    
    std::unique_ptr<QLabel> iconLabel;
    std::unique_ptr<QLabel> titleLabel;
    
    std::unique_ptr<QLineEdit> usernameInput;
    std::unique_ptr<QLineEdit> passwordInput;
    
    std::unique_ptr<QPushButton> loginButton;
    std::unique_ptr<QPushButton> createAccountButton;
    std::unique_ptr<QPushButton> passwordToggleButton;

    std::unique_ptr<SignUpWindow> signUpWindow_;
    
    std::unique_ptr<QVBoxLayout> mainLayout;
    
    bool passwordVisible = false;
};