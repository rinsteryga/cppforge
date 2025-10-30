#include "AuthManager.hpp"

#include <QDebug>

namespace cppforge
{
    namespace services
    {

        AuthManager::AuthManager(QObject *parent)
            : QObject(parent)
        {
            // не готово
        }

        bool AuthManager::login(const QString &username_, const QString &password_)
        {
            if (users_.contains(username_) && users_[username_] == password_)
            {
                currentUser_ = username_;
                qDebug() << "User signed in:" << currentUser_;
                return true;
            }
            qDebug() << "Sign in failed for user:" << username_;
            return false;
        }

        bool AuthManager::registerUser(const QString &username_, const QString &email_, const QString &password_)
        {
            if (users_.contains(username_))
            {
                qDebug() << "Sign up failed. User already exists:" << username_;
                return false;
            }
            users_.insert(username_, password_);
            qDebug() << "User signed up successfully:" << username_;
            return true;
        }
    } // namespace services
} // namespace cppforge
