#include "../../include/services/AuthManager.hpp"

#include "../../include/services/SessionManager.hpp"
#include "../../include/utils/security/PasswordHashGenerator.hpp"

#include <QDebug>

#include <chrono>

namespace cppforge::services
{
    AuthManager::AuthManager(std::unique_ptr<repositories::IUserRepository> userRepository, QObject *parent)
        : QObject(parent), userRepository_(std::move(userRepository))
    {
    }

    bool AuthManager::login(const QString &email, const QString &password)
    {
        auto userOpt = userRepository_->findByEmail(email);
        if (!userOpt.has_value())
        {
            userOpt = userRepository_->findByUsername(email);
        }

        if (!userOpt.has_value())
        {
            return false;
        }

        if (!cppforge::utils::security::PasswordHashGenerator::verify(password, userOpt->getPasswordHash(),
                                                                      userOpt->getSalt()))
        {
            return false;
        }

        SessionManager::instance().loginUser(std::make_shared<cppforge::entities::User>(std::move(userOpt.value())));
        return true;
    }

    bool AuthManager::registerUser(const QString &username, const QString &email, const QString &password)
    {
        if (userRepository_->findByEmail(email).has_value())
        {
            return false;
        }

        const QString salt = cppforge::utils::security::PasswordHashGenerator::generateSalt();
        const QString hash = cppforge::utils::security::PasswordHashGenerator::generate(password, salt);

        cppforge::entities::User newUser(0, username, email, hash, salt, std::chrono::system_clock::now());

        return userRepository_->save(newUser);
    }
} // namespace cppforge::services
