#pragma once

#include "../../core/include/services/AuthManager.hpp"
#include "../../core/include/services/ThemeService.hpp"
#include "CustomTitleBar.hpp"
#include "SignUpWindow.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

/**
 * @brief The login and authentication window.
 *
 * AuthWindow provides the initial entry point for users to log into their
 * accounts or navigate to the registration screen. It leverages AuthManager
 * for secure credential verification and session management.
 */
class AuthWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new AuthWindow instance.
     * @param authManager Shared pointer to the authentication manager service.
     * @param parent The parent QWidget.
     */
    explicit AuthWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent = nullptr);

    /**
     * @brief Default destructor.
     */
    ~AuthWindow();

    /**
     * @brief Triggers a fade-in animation for the window.
     */
    void fadeIn();
    void setThemeService(cppforge::services::ThemeService *service);

signals:
    void loginSuccessful();
    void switchToMainMenu(const QString &username, int userId);

private:
    void setupUI();
    void setupTitleBar();
    void setupLogo();
    void setupTitle();
    void setupInputFields();
    void setupWindowProperties();
    void setupLoginButton();
    void setupCreateAccountLink();
    void setupLayout();
    void setupStyles();
    void showFallbackLogo();
    void centerWindow();
    void togglePasswordVisibility();

    void openSignUpWindow();
    void onLoginClicked();
    void onCreateAccountClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

    std::unique_ptr<QLabel> iconLabel_;
    std::unique_ptr<QLabel> titleLabel_;
    std::unique_ptr<CustomTitleBar> customTitleBar_;

    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> passwordInput_;

    std::unique_ptr<QPushButton> loginButton_;
    std::unique_ptr<QPushButton> createAccountButton_;
    std::unique_ptr<QPushButton> passwordToggleButton_;

    std::unique_ptr<SignUpWindow> signUpWindow_;
    std::unique_ptr<QVBoxLayout> mainLayout_;

    cppforge::services::ThemeService *themeService_ = nullptr;
    std::shared_ptr<cppforge::services::AuthManager> authManager_;
    bool passwordVisible_ = false;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
};