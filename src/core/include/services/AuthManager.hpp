#pragma once

#include "../entities/User.hpp"
#include "../repositories/IUserRepository.hpp"

#include <QObject>
#include <QString>

#include <memory>

namespace cppforge::services
{
    /**
     * @brief Manages the user authentication lifecycle, including login and registration.
     * 
     * AuthManager acts as a high-level service that coordinates with the IUserRepository
     * to validate credentials and create new user accounts. It inherits from QObject
     * to facilitate signal-slot communication in the GUI.
     */
    class AuthManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs an AuthManager with a specific user repository.
         * 
         * @param userRepository A unique pointer to an implementation of IUserRepository.
         * @param parent The optional parent QObject.
         */
        explicit AuthManager(std::unique_ptr<repositories::IUserRepository> userRepository, QObject *parent = nullptr);

        /**
         * @brief Attempts to authenticate a user with their email and password.
         * 
         * @param email The user's registered email address.
         * @param password The raw password string to be verified against the stored hash.
         * @return True if authentication succeeded; otherwise false.
         */
        bool login(const QString &email, const QString &password);

        /**
         * @brief Registers a new user account in the system.
         * 
         * @param username The chosen display name for the new profile.
         * @param email The unique email address for the new account.
         * @param password The raw password to be hashed and stored.
         * @return True if the user was successfully registered; false if the email/username 
         *         is taken or saving failed.
         */
        bool registerUser(const QString &username, const QString &email, const QString &password);

    private:
        std::unique_ptr<repositories::IUserRepository> userRepository_;
    };

} // namespace cppforge::services
