#pragma once

#include "../entities/User.hpp"

#include <QObject>

#include <memory>

namespace cppforge::services
{
    /**
     * @brief Singleton service managing the active user session and state.
     * 
     * SessionManager keeps track of the currently authenticated User entity,
     * emits signals for session state changes (login/logout), and provides
     * a global access point to the active profile.
     */
    class SessionManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Returns the global singleton instance of the SessionManager.
         * @return A reference to the SessionManager instance.
         */
        static SessionManager &instance();

        /**
         * @brief Sets the given user as the active session holder.
         * @param user A shared pointer to the User entity being logged in.
         */
        void loginUser(std::shared_ptr<cppforge::entities::User> user);

        /**
         * @brief Clears the active session and logs out the current user.
         */
        void logoutUser();

        /**
         * @brief Checks if there is an active authenticated session.
         * @return True if a user is currently logged in; otherwise false.
         */
        bool isAuthenticated() const;

        /**
         * @brief Retrieves the user entity currently tied to the session.
         * @return A shared pointer to the current User; null if not authenticated.
         */
        std::shared_ptr<cppforge::entities::User> getCurrentUser() const;

    signals:
        /**
         * @brief Emitted when a new session is successfully established.
         */
        void userLoggedIn();

        /**
         * @brief Emitted when the current session is terminated.
         */
        void userLoggedOut();

        /**
         * @brief Emitted when the current user's profile metadata changes.
         */
        void userProfileUpdated();

    private:
        explicit SessionManager(QObject *parent = nullptr);
        ~SessionManager() override;
        Q_DISABLE_COPY(SessionManager)

        std::shared_ptr<cppforge::entities::User> currentUser_;
    };
} // namespace cppforge::services
