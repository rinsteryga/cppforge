#pragma once

#include "../../../core/include/repositories/IUserRepository.hpp"

#include <QString>
#include <QtSql/QSqlDatabase>

#include <cstdint>
#include <optional>
#include <vector>

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

        /**
         * @brief Retrieves the count of unique coding tasks a user has solved.
         * @param userId The ID of the user.
         * @return The count of unique successfully solved coding tasks.
         */
        int getSolvedTasksCount(uint64_t userId) const override;
        int getCompletedLessonsCount(uint64_t userId) const override;
        int getAchievementsCount(uint64_t userId) const override;
        int getStreak(uint64_t userId) const override;
        int getTotalSubmissionsCount(uint64_t userId) const override;
        std::vector<Activity> getRecentActivity(uint64_t userId, int limit) const override;
        std::vector<uint64_t> getEarnedAchievementIds(uint64_t userId) const override;
        void updateStreak(uint64_t userId) override;

    private:
        QSqlDatabase &database_;

        void loadUserAchievements(entities::User &user) const;
        void loadUserCompletedLevels(entities::User &user) const;
        void saveUserAchievements(const entities::User &user);
    };
} // namespace cppforge::repositories
