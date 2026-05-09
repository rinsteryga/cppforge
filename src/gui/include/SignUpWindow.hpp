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

/**
 * @class SignUpWindow
 * @brief Window for user registration.
 *
 * Provides fields for username, email, and password, and handles
 * the interaction with AuthManager to create new accounts.
 */
class SignUpWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a SignUpWindow.
     * @param authManager Shared pointer to the authentication manager.
     * @param parent Parent widget.
     */
    explicit SignUpWindow(std::shared_ptr<cppforge::services::AuthManager> authManager, QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~SignUpWindow() override;

    /**
     * @brief Triggers a fade-in animation.
     */
    void fadeIn();

    /**
     * @brief Sets the theme service and connects to its signals.
     * @param service Pointer to the ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

signals:
    /**
     * @brief Emitted when the user requests to go back to the login screen.
     */
    void switchToLogin();

private slots:
    /**
     * @brief Processes the sign-up request.
     */
    void onSignUpButtonClicked();

    /**
     * @brief Emits switchToLogin signal.
     */
    void onBackToLoginClicked();

    /**
     * @brief Toggles between masked and plain text for the password field.
     */
    void togglePasswordVisibility();

protected:
    /**
     * @brief Paints the background and rounded corners.
     * @param event Paint event.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Event filter for handling internal layout adjustments.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * @brief Initializes the UI components.
     */
    void setupUI();

    /**
     * @brief Configures the custom title bar.
     */
    void setupTitleBar();

    /**
     * @brief Sets basic window properties.
     */
    void setupWindowProperties();

    /**
     * @brief Centers the window on the screen.
     */
    void centerWindow();

    /**
     * @brief Configures the logo display.
     */
    void setupLogo();

    /**
     * @brief Configures the title label.
     */
    void setupTitle();

    /**
     * @brief Configures the input fields.
     */
    void setupInputFields();

    /**
     * @brief Configures the sign-up button.
     */
    void setupSignUpButton();

    /**
     * @brief Configures the link back to login.
     */
    void setupBackToLoginLink();

    /**
     * @brief Assembles components into the layout.
     */
    void setupLayout();

    /**
     * @brief Applies CSS styles based on the current theme.
     */
    void setupStyles();

    /**
     * @brief Connects signals and slots.
     */
    void setupConnections();

    /**
     * @brief Shows a fallback logo if resource loading fails.
     */
    void showFallbackLogo();

    std::unique_ptr<QLabel> iconLabel_;                            ///< Logo icon label.
    std::unique_ptr<QLabel> titleLabel_;                           ///< Window title label.
    std::unique_ptr<CustomTitleBar> customTitleBar_;               ///< Custom title bar.
    std::unique_ptr<QLineEdit> usernameInput_;                     ///< Username input field.
    std::unique_ptr<QLineEdit> emailInput_;                        ///< Email input field.
    std::unique_ptr<QLineEdit> passwordInput_;                     ///< Password input field.
    std::unique_ptr<QPushButton> signUpButton_;                    ///< Sign-up button.
    std::unique_ptr<QPushButton> backToLoginButton_;               ///< Back to login button.
    std::unique_ptr<QPushButton> passwordToggleButton_;            ///< Password visibility toggle.
    std::unique_ptr<QVBoxLayout> mainLayout_;                      ///< Main layout.
    QWidget *centerContainer_ = nullptr;                           ///< Inner container for scrolling.
    cppforge::services::ThemeService *themeService_ = nullptr;     ///< Theme service.
    std::shared_ptr<cppforge::services::AuthManager> authManager_; ///< Auth manager.
    bool passwordVisible_ = false;                                 ///< Password visibility flag.
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;      ///< Transition animation.
};