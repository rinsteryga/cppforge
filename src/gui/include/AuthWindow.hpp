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
 * @class AuthWindow
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
     * @brief Destructor.
     */
    ~AuthWindow() override;

    /**
     * @brief Triggers a fade-in animation for the window.
     */
    void fadeIn();

    /**
     * @brief Connects the window to the theme service for dynamic style updates.
     * @param service Pointer to the application-wide ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

signals:
    /**
     * @brief Emitted when login is successful.
     */
    void loginSuccessful();

    /**
     * @brief Emitted when user should be transitioned to the main menu.
     * @param username The authenticated username.
     * @param userId The unique identifier of the user.
     */
    void switchToMainMenu(const QString &username, int userId);

private slots:
    /**
     * @brief Opens the registration window.
     */
    void openSignUpWindow();

    /**
     * @brief Slot called when the login button is clicked.
     */
    void onLoginClicked();

    /**
     * @brief Slot called when the create account button is clicked.
     */
    void onCreateAccountClicked();

    /**
     * @brief Toggles password echo mode between password and normal text.
     */
    void togglePasswordVisibility();

protected:
    /**
     * @brief Handles the background painting of the window.
     * @param event The paint event object.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Event filter for handling internal layout adjustments.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * @brief Handles key press events for shortcuts.
     */
    void keyPressEvent(QKeyEvent *event) override;

private:
    /**
     * @brief Initializes the main UI layout and components.
     */
    void setupUI();

    /**
     * @brief Configures the custom title bar.
     */
    void setupTitleBar();

    /**
     * @brief Configures the application logo display.
     */
    void setupLogo();

    /**
     * @brief Sets up the title text label.
     */
    void setupTitle();

    /**
     * @brief Configures username and password input fields.
     */
    void setupInputFields();

    /**
     * @brief Sets basic window properties (size, flags).
     */
    void setupWindowProperties();

    /**
     * @brief Configures the login button and its styles.
     */
    void setupLoginButton();

    /**
     * @brief Configures the "Create Account" link/button.
     */
    void setupCreateAccountLink();

    /**
     * @brief Assembles all components into the main layout.
     */
    void setupLayout();

    /**
     * @brief Applies global CSS styles to the window.
     */
    void setupStyles();

    /**
     * @brief Shows a fallback logo if the main one fails to load.
     */
    void showFallbackLogo();

    /**
     * @brief Centers the window on the primary screen.
     */
    void centerWindow();

    std::unique_ptr<QLabel> iconLabel_;                            ///< Application icon label.
    std::unique_ptr<QLabel> titleLabel_;                           ///< Application title label.
    std::unique_ptr<CustomTitleBar> customTitleBar_;               ///< Custom title bar.
    std::unique_ptr<QLineEdit> usernameInput_;                     ///< Username input field.
    std::unique_ptr<QLineEdit> passwordInput_;                     ///< Password input field.
    std::unique_ptr<QPushButton> loginButton_;                     ///< Login action button.
    std::unique_ptr<QPushButton> createAccountButton_;             ///< Transition to sign-up button.
    std::unique_ptr<QPushButton> passwordToggleButton_;            ///< Password visibility toggle button.
    std::unique_ptr<SignUpWindow> signUpWindow_;                   ///< Registration window instance.
    std::unique_ptr<QVBoxLayout> mainLayout_;                      ///< Main window layout.
    QWidget *centerContainer_ = nullptr;                           ///< Inner container for scrolling.
    cppforge::services::ThemeService *themeService_ = nullptr;     ///< Theme service.
    std::shared_ptr<cppforge::services::AuthManager> authManager_; ///< Auth manager.
    bool passwordVisible_ = false;                                 ///< Password visibility state.
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;      ///< Fade transition animation.
};