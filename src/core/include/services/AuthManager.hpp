#pragma once

#include <QObject>
#include <QString>
#include <QHash>

namespace cppforge
{
    namespace services
    {
        class AuthManager : public QObject
        {
            Q_OBJECT

        public:
            AuthManager(QObject *parent = nullptr);

            bool login(const QString &username_, const QString &password_);
            void logout() { currentUser_.clear(); }
            bool registerUser(const QString &username_, const QString &email_, const QString &password_);

            bool isAuthenticated() const { return !currentUser_.isEmpty(); }
            QString getCurrentUser() const { return currentUser_; };

        private:
            QHash<QString, QString> users_;
            QString currentUser_;
        };

    } // namespace services
} // namespace cppforge
