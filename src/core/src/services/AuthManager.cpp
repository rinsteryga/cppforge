#include "../../include/services/AuthManager.hpp"
#include "../../include/utils/security/PasswordHashGenerator.hpp"

namespace cppforge::services
{
    AuthManager::AuthManager(QObject *parent)
        : QObject(parent) {}

    bool AuthManager::login(const QString &username, const QString &password)
    {
        if (!users_.contains(username))
            return false;

        const QString storedHash = users_.value(username);
        if (!cppforge::utils::security::PasswordHashGenerator::verify(password, storedHash))
            return false;

        currentUser_ = username;
        return true;
    }

    bool AuthManager::registerUser(const QString &username, const QString &email, const QString &password)
    {
        Q_UNUSED(email);

        if (users_.contains(username))
            return false;

        const QString hash =  cppforge::utils::security::PasswordHashGenerator::generate(password);
        users_.insert(username, hash);
        return true;
    }
}
