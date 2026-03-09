#pragma once

#include "../entities/User.hpp"

#include <QObject>

#include <memory>

namespace cppforge
{
    namespace services
    {
        class SessionManager : public QObject
        {
            Q_OBJECT

        public:
            static SessionManager &instance();

            void loginUser(std::shared_ptr<cppforge::entities::User> user);
            void logoutUser();

            bool isAuthenticated() const;
            std::shared_ptr<cppforge::entities::User> getCurrentUser() const;

        signals:
            void userLoggedIn();
            void userLoggedOut();
            void userProfileUpdated();

        private:
            explicit SessionManager(QObject *parent = nullptr);
            ~SessionManager() override;
            Q_DISABLE_COPY(SessionManager)

            std::shared_ptr<cppforge::entities::User> currentUser_;
        };
    } // namespace services
} // namespace cppforge
