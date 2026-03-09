#pragma once

#include "../entities/User.hpp"
#include "../repositories/IUserRepository.hpp"

#include <QObject>
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
            explicit AuthManager(std::unique_ptr<repositories::IUserRepository> userRepository,
                                 QObject *parent = nullptr);

            bool login(const QString &email, const QString &password);
            bool registerUser(const QString &username, const QString &email, const QString &password);

        private:
            std::unique_ptr<repositories::IUserRepository> userRepository_;
        };

    } // namespace services
} // namespace cppforge
