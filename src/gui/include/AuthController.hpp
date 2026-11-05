#pragma once

#include "../../core/include/services/AuthManager.hpp"
#include "../../core/include/services/SessionManager.hpp"

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QString>

#include <memory>

namespace cppforge::gui
{
    /**
     * @class AuthController
     * @brief Handles user authentication and registration for QML views.
     */
    class AuthController : public QObject
    {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingChanged)
        Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
        Q_PROPERTY(QString successMessage READ successMessage NOTIFY successMessageChanged)

    public:
        explicit AuthController(QObject *parent = nullptr);
        ~AuthController() override = default;

        void setAuthManager(std::shared_ptr<cppforge::services::AuthManager> authManager);

        bool isLoading() const { return m_isLoading; }
        QString errorMessage() const { return m_errorMessage; }
        QString successMessage() const { return m_successMessage; }

        Q_INVOKABLE bool login(const QString &username, const QString &password, bool rememberMe = true);
        Q_INVOKABLE bool registerUser(const QString &username, const QString &email, const QString &password);
        Q_INVOKABLE void clearMessages();

    signals:
        void loadingChanged();
        void errorMessageChanged();
        void successMessageChanged();
        void loginSucceeded(const QString &username, int userId);
        void registrationSucceeded();

    private:
        void setErrorMessage(const QString &message);
        void setSuccessMessage(const QString &message);
        void setLoading(bool loading);

        std::shared_ptr<cppforge::services::AuthManager> m_authManager;
        bool m_isLoading = false;
        QString m_errorMessage;
        QString m_successMessage;
    };
} // namespace cppforge::gui
