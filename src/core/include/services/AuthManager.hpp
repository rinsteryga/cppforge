#pragma once

#include "../repositories/IUserRepository.hpp"
#include "../entities/User.hpp"

#include <QObject>
#include <QString>
#include <QString>
#include <memory>


namespace cppforge
{
    namespace services
    {
        class AuthManager : public QObject
        {
            Q_OBJECT

        public:
            explicit AuthManager(std::unique_ptr<repositories::IUserRepository> userRepository, QObject *parent = nullptr);

            bool login(const QString &email, const QString &password);
            void logout() { currentUser_.reset(); }
            bool registerUser(const QString &username, const QString &email, const QString &password);

            bool isAuthenticated() const { return currentUser_.has_value(); }
            std::optional<cppforge::entities::User> getCurrentUser() const { return currentUser_; }

        private:
            std::unique_ptr<repositories::IUserRepository> userRepository_;
            std::optional<cppforge::entities::User> currentUser_;
        };

    } // namespace services
} // namespace cppforge
