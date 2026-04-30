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
         * @brief Fetches a user by their ID.
         * @param id The ID to search for.
         * @return User if found, std::nullopt otherwise.
         */
        std::optional<entities::User> findById(uint64_t id) const override;

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

        /**
         * @brief Retrieves the count of lessons a user has completed.
         * @param userId The ID of the user.
         * @return The count of completed lessons.
         */
        int getCompletedLessonsCount(uint64_t userId) const override;

        /**
         * @brief Retrieves the count of achievements a user has earned.
         * @param userId The ID of the user.
         * @return The count of earned achievements.
         */
        int getAchievementsCount(uint64_t userId) const override;

        /**
         * @brief Retrieves the current streak of a user.
         * @param userId The ID of the user.
         * @return The current streak in days.
         */
        int getStreak(uint64_t userId) const override;

        /**
         * @brief Retrieves the total number of submissions made by a user.
         * @param userId The ID of the user.
         * @return The total submission count.
         */
        int getTotalSubmissionsCount(uint64_t userId) const override;

        /**
         * @brief Retrieves the last N activities of a user.
         * @param userId The ID of the user.
         * @param limit Maximum number of activities to return.
         * @return A vector of Activity objects.
         */
        std::vector<Activity> getRecentActivity(uint64_t userId, int limit) const override;

        /**
         * @brief Retrieves the IDs of all achievements earned by a user.
         * @param userId The ID of the user.
         * @return A vector of achievement IDs.
         */
        std::vector<uint64_t> getEarnedAchievementIds(uint64_t userId) const override;

        /**
         * @brief Updates the user's streak and last solved timestamp in the database.
         * @param userId The ID of the user to update.
         */
        void updateStreak(uint64_t userId) override;

        /**
         * @brief Updates the user's avatar path in the database.
         * @param userId The ID of the user to update.
         * @param avatarPath The new path to the avatar image.
         */
        void updateAvatar(uint64_t userId, const QString &avatarPath) override;

        bool saveLessonProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId, bool isCompleted) override;
        bool saveSubmission(uint64_t userId, uint64_t moduleId, uint64_t taskId, const QString &code,
                            bool isSuccess) override;

    private:
        QSqlDatabase &database_;

        /**
         * @brief Lazy-loads achievement IDs for a user entity.
         * @param user The User entity to populate.
         */
        void loadUserAchievements(entities::User &user) const;

        /**
         * @brief Lazy-loads completed level IDs for a user entity.
         * @param user The User entity to populate.
         */
        void loadUserCompletedLevels(entities::User &user) const;

        /**
         * @brief Persists the user's achievements to the database.
         * @param user The User entity whose achievements should be saved.
         */
        void saveUserAchievements(const entities::User &user);
    };
} // namespace cppforge::repositories
