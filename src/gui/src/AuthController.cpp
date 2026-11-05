#include "AuthController.hpp"

#include <QRegularExpression>

namespace cppforge::gui
{
    AuthController::AuthController(QObject *parent)
        : QObject(parent)
    {
    }

    void AuthController::setAuthManager(std::shared_ptr<cppforge::services::AuthManager> authManager)
    {
        m_authManager = std::move(authManager);
    }

    void AuthController::setLoading(bool loading)
    {
        if (m_isLoading != loading)
        {
            m_isLoading = loading;
            emit loadingChanged();
        }
    }

    void AuthController::setErrorMessage(const QString &message)
    {
        if (m_errorMessage != message)
        {
            m_errorMessage = message;
            emit errorMessageChanged();
        }
    }

    void AuthController::setSuccessMessage(const QString &message)
    {
        if (m_successMessage != message)
        {
            m_successMessage = message;
            emit successMessageChanged();
        }
    }

    void AuthController::clearMessages()
    {
        setErrorMessage("");
        setSuccessMessage("");
    }

    bool AuthController::login(const QString &username, const QString &password, bool rememberMe)
    {
        clearMessages();

        QString trimmedUser = username.trimmed();
        if (trimmedUser.isEmpty() || password.isEmpty())
        {
            setErrorMessage("Please fill in both username and password.");
            return false;
        }

        if (!m_authManager)
        {
            setErrorMessage("Authentication service unavailable.");
            return false;
        }

        setLoading(true);
        bool ok = m_authManager->login(trimmedUser, password);
        setLoading(false);

        if (ok)
        {
            int userId = -1;
            auto currentUser = cppforge::services::SessionManager::instance().getCurrentUser();
            if (currentUser)
            {
                userId = static_cast<int>(currentUser->getId());
            }

            QSettings settings("CppForge", "StudyApp");
            settings.setValue("auth/remember", rememberMe);
            settings.setValue("auth/user_id", userId);
            settings.setValue("auth/username", trimmedUser);

            emit loginSucceeded(trimmedUser, userId);
            return true;
        }

        setErrorMessage("Invalid username or password.");
        return false;
    }

    bool AuthController::registerUser(const QString &username, const QString &email, const QString &password)
    {
        clearMessages();

        QString trimmedUser = username.trimmed();
        QString trimmedEmail = email.trimmed();

        if (trimmedUser.isEmpty() || trimmedEmail.isEmpty() || password.isEmpty())
        {
            setErrorMessage("All fields are required.");
            return false;
        }

        static const QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,20}$");
        if (!usernameRegex.match(trimmedUser).hasMatch())
        {
            setErrorMessage("Username must be 3-20 characters (letters, numbers, underscores).");
            return false;
        }

        static const QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.[a-zA-Z]{2,}$");
        if (!emailRegex.match(trimmedEmail).hasMatch())
        {
            setErrorMessage("Please enter a valid email address.");
            return false;
        }

        if (password.length() < 6)
        {
            setErrorMessage("Password must be at least 6 characters long.");
            return false;
        }

        if (!m_authManager)
        {
            setErrorMessage("Authentication service unavailable.");
            return false;
        }

        setLoading(true);
        bool ok = m_authManager->registerUser(trimmedUser, trimmedEmail, password);
        setLoading(false);

        if (ok)
        {
            setSuccessMessage("Account created successfully! You can now log in.");
            emit registrationSucceeded();
            return true;
        }

        setErrorMessage("Registration failed. Username or email may already be taken.");
        return false;
    }
} // namespace cppforge::gui
