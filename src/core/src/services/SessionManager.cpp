#include "../../include/services/SessionManager.hpp"

namespace cppforge::services
{
    SessionManager &SessionManager::instance()
    {
        static SessionManager instance;
        return instance;
    }

    SessionManager::SessionManager(QObject *parent) : QObject(parent) {}

    SessionManager::~SessionManager() = default;

    void SessionManager::loginUser(std::shared_ptr<cppforge::entities::User> user)
    {
        currentUser_ = std::move(user);
        emit userLoggedIn();
    }

    void SessionManager::logoutUser()
    {
        currentUser_.reset();
        emit userLoggedOut();
    }

    bool SessionManager::isAuthenticated() const
    {
        return currentUser_ != nullptr;
    }

    std::shared_ptr<cppforge::entities::User> SessionManager::getCurrentUser() const
    {
        return currentUser_;
    }
} // namespace cppforge::services
