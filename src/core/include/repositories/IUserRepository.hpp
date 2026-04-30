#pragma once

#include <QString>

#include <cstdint>
#include <optional>
#include <vector>

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
            IUserRepository(const IUserRepository &) = delete;
            IUserRepository &operator=(const IUserRepository &) = delete;
            IUserRepository(IUserRepository &&) = delete;
            IUserRepository &operator=(IUserRepository &&) = delete;

            /**
             * @brief Searches for a user profile by their unique ID.
             *
             * @param id The ID to look for.
             * @return An std::optional containing the User if found; otherwise std::nullopt.
             */
            virtual std::optional<entities::User> findById(uint64_t id) const = 0;

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

            /**
             * @brief Retrieves the count of unique coding tasks a user has solved.
             *
             * @param userId The ID of the user.
             * @return The count of unique successfully solved coding tasks.
             */
            virtual int getSolvedTasksCount(uint64_t userId) const = 0;

            /**
             * @brief Retrieves the count of lessons a user has completed.
             *
             * @param userId The ID of the user.
             * @return The count of completed lessons.
             */
            virtual int getCompletedLessonsCount(uint64_t userId) const = 0;

            /**
             * @brief Retrieves the count of achievements a user has earned.
             *
             * @param userId The ID of the user.
             * @return The count of earned achievements.
             */
            virtual int getAchievementsCount(uint64_t userId) const = 0;

            /**
             * @brief Retrieves the current streak of a user.
             *
             * @param userId The ID of the user.
             * @return The current streak in days.
             */
            virtual int getStreak(uint64_t userId) const = 0;

            /**
             * @brief Retrieves the total number of submissions made by a user.
             *
             * @param userId The ID of the user.
             * @return The total submission count.
             */
            virtual int getTotalSubmissionsCount(uint64_t userId) const = 0;

            struct Activity
            {
                QString title;
                QString type;
                QString date;
            };

            /**
             * @brief Retrieves the last N activities of a user.
             *
             * @param userId The ID of the user.
             * @param limit Maximum number of activities to return.
             * @return A vector of Activity objects.
             */
            virtual std::vector<Activity> getRecentActivity(uint64_t userId, int limit) const = 0;

            /**
             * @brief Retrieves the IDs of all achievements earned by a user.
             *
             * @param userId The ID of the user.
             * @return A vector of achievement IDs.
             */
            virtual std::vector<uint64_t> getEarnedAchievementIds(uint64_t userId) const = 0;

            /**
             * @brief Updates the user's streak and last solved timestamp.
             * @param userId The ID of the user.
             */
            virtual void updateStreak(uint64_t userId) = 0;

            /**
             * @brief Updates the user's avatar path.
             *
             * @param userId The ID of the user.
             * @param avatarPath The new path to the avatar image.
             */
            virtual void updateAvatar(uint64_t userId, const QString &avatarPath) = 0;

            /**
             * @brief Saves or updates the progression state of a lesson.
             * @param userId The ID of the user.
             * @param moduleId The ID of the module.
             * @param lessonId The ID of the lesson.
             * @param isCompleted True if the user successfully completed the lesson.
             * @return True if saved successfully.
             */
            virtual bool saveLessonProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId,
                                            bool isCompleted) = 0;

            /**
             * @brief Records a user's code submission for a task.
             * @param userId The ID of the user.
             * @param moduleId The ID of the module.
             * @param taskId The ID of the coding task.
             * @param code The source code submitted.
             * @param isSuccess True if the submission passed all tests.
             * @return True if saved successfully.
             */
            virtual bool saveSubmission(uint64_t userId, uint64_t moduleId, uint64_t taskId, const QString &code,
                                        bool isSuccess) = 0;

            /**
             * @brief Retrieves the source code of the most recent submission for a task.
             * @param userId The ID of the user.
             * @param taskId The ID of the coding task.
             * @return The source code if found, std::nullopt otherwise.
             */
            virtual std::optional<QString> getLastSubmission(uint64_t userId, uint64_t taskId) const = 0;

            /**
             * @brief Checks if a specific user has completed a specific lesson.
             * @param userId The ID of the user.
             * @param lessonId The ID of the lesson.
             * @return True if completed.
             */
            virtual bool isLessonCompleted(uint64_t userId, uint64_t lessonId) const = 0;

            /**
             * @brief Retrieves the IDs of all lessons completed by a user in a module.
             * @param userId The ID of the user.
             * @param moduleId The ID of the module.
             * @return A vector of completed lesson IDs.
             */
            virtual std::vector<uint64_t> getCompletedLessonIds(uint64_t userId, uint64_t moduleId) const = 0;

            /**
             * @brief Calculates the completion progress percentage for a specific module.
             * @param userId The ID of the user.
             * @param moduleId The ID of the module.
             * @return The completion percentage (0-100).
             */
            virtual int getModuleProgress(uint64_t userId, uint64_t moduleId) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
