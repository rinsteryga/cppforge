#include "../../include/services/AuthManager.hpp"
#include "../../include/utils/security/PasswordHashGenerator.hpp"

#include <chrono>

namespace cppforge::services
{
    AuthManager::AuthManager(std::unique_ptr<repositories::IUserRepository> userRepository, QObject *parent)
        : QObject(parent), userRepository_(std::move(userRepository)) {}

    bool AuthManager::login(const QString &email, const QString &password)
    {
        auto userOpt = userRepository_->findByEmail(email);
        if (!userOpt.has_value()) {
            userOpt = userRepository_->findByUsername(email);
        }

        if (!userOpt.has_value())
            return false;

        if (!cppforge::utils::security::PasswordHashGenerator::verify(password, userOpt->getPasswordHash()))
            return false;

        currentUser_ = userOpt.value();
        return true;
    }

    bool AuthManager::registerUser(const QString &username, const QString &email, const QString &password)
    {
        if (userRepository_->findByEmail(email).has_value())
            return false;

        const QString hash = cppforge::utils::security::PasswordHashGenerator::generate(password);
        
        cppforge::entities::User newUser(0, username, email, hash, std::chrono::system_clock::now());
        
        return userRepository_->save(newUser);
    }
}
