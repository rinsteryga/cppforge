#pragma once

#include "../../../core/include/repositories/IUserRepository.hpp"

#include <QString>
#include <QtSql/QSqlDatabase>

#include <optional>

namespace cppforge::repositories
{
    /**
     * @brief PostgreSQL-backed implementation of the User repository.
     *
     * Handles persistence and retrieval of user data from the PostgreSQL database
     * using Qt's SQL module.
     */
    class PgUserRepository : public IUserRepository
    {
    public:
        /**
         * @brief Constructs a PgUserRepository with a database connection.
         * @param database Reference to an open QSqlDatabase.
         */
        explicit PgUserRepository(QSqlDatabase &database);

        /**
         * @brief Fetches a user by their email.
         * @param email The email to search for.
         * @return User if found, std::nullopt otherwise.
         */
        std::optional<entities::User> findByEmail(const QString &email) const override;

        /**
         * @brief Fetches a user by their username.
         * @param username The name to search for.
         * @return User if found, std::nullopt otherwise.
         */
        std::optional<entities::User> findByUsername(const QString &username) const override;

        /**
         * @brief Saves or updates a user in the database.
         * @param user The User entity to persist.
         * @return True if successful, false otherwise.
         */
        bool save(const entities::User &user) override;

    private:
        QSqlDatabase &database_;
    };
} // namespace cppforge::repositories
