#pragma once

#include <QString>

#include <optional>

namespace cppforge
{
    namespace entities
    {
        class User;
    }

    namespace repositories
    {

        /**
         * @brief Defines the abstract data access contract for User entities.
         * 
         * Manages user profile retrieval and persistence, providing the primary
         * interface for authentication and profile management logic.
         */
        class IUserRepository
        {
        public:
            IUserRepository() = default;
            virtual ~IUserRepository() = default;
            IUserRepository(const IUserRepository&) = delete;
            IUserRepository& operator=(const IUserRepository&) = delete;
            IUserRepository(IUserRepository&&) = delete;
            IUserRepository& operator=(IUserRepository&&) = delete;

            /**
             * @brief Searches for a user profile by their unique email address.
             * 
             * @param email The registered email string to look for.
             * @return An std::optional containing the User if found; otherwise std::nullopt.
             */
            virtual std::optional<entities::User> findByEmail(const QString &email) const = 0;

            /**
             * @brief Searches for a user profile by their display username.
             * 
             * @param username The name to look for.
             * @return An std::optional containing the User if found; otherwise std::nullopt.
             */
            virtual std::optional<entities::User> findByUsername(const QString &username) const = 0;

            /**
             * @brief Persists a user's state changes or creates a new user record.
             * 
             * @param user The User entity to save.
             * @return True if the operation succeeded; otherwise false.
             */
            virtual bool save(const entities::User &user) = 0;
        };
    } // namespace repositories
} // namespace cppforge
