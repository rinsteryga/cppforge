#pragma once

#include "../entities/User.hpp"
#include "../repositories/IAchievementRepository.hpp"
#include "../repositories/IUserRepository.hpp"

#include <cstdint>
#include <optional>

namespace cppforge::services
{
    /**
     * @brief Service providing high-level operations on user profiles and statistics.
     *
     * UserService acts as a bridge between the data layer (IUserRepository) and the
     * application's business needs, such as retrieving solved task counts and
     * managing user metadata.
     */
    class UserService
    {
    public:
        /**
         * @brief Constructs a UserService with a user repository.
         * @param userRepository A pointer to an implementation of IUserRepository.
         */
        explicit UserService(repositories::IUserRepository &userRepository,
                             repositories::IAchievementRepository &achievementRepository);

        /**
         * @brief Retrieves a full User entity by username.
         * @param username The name to search for.
         * @return An optional containing the User if found.
         */
        std::optional<entities::User> getUser(const QString &username) const;

        /**
         * @brief Retrieves the total number of unique coding tasks solved by a user.
         * @param userId The ID of the user.
         * @return The count of unique successfully solved coding tasks.
         */
        int getSolvedTasksCount(uint64_t userId) const;

        /**
         * @brief Retrieves the number of lessons completed by a user.
         * @param userId The ID of the user.
         * @return The count of completed lessons.
         */
        int getCompletedLessonsCount(uint64_t userId) const;

        /**
         * @brief Retrieves the number of achievements earned by a user.
         * @param userId The ID of the user.
         * @return The count of earned achievements.
         */
        int getAchievementsCount(uint64_t userId) const;

        /**
         * @brief Retrieves the current streak of a user.
         * @param userId The ID of the user.
         * @return The current streak in days.
         */
        int getStreak(uint64_t userId) const;

        /**
         * @brief Retrieves the total number of submissions made by a user.
         * @param userId The ID of the user.
         * @return The total submission count.
         */
        int getTotalSubmissionsCount(uint64_t userId) const;

        using Activity = repositories::IUserRepository::Activity;

        /**
         * @brief Retrieves the last N activities of a user.
         * @param userId The ID of the user.
         * @param limit Maximum number of activities to return.
         * @return A vector of Activity objects.
         */
        std::vector<Activity> getRecentActivity(uint64_t userId, int limit) const;

        /**
         * @brief Retrieves all available achievements with their status for a specific user.
         *
         * @param userId The ID of the user.
         * @return A vector of Achievement entities, each with earned_at set if earned.
         */
        std::vector<entities::Achievement> getAllAchievementsStatus(uint64_t userId) const;

    private:
        repositories::IUserRepository &userRepository_;
        repositories::IAchievementRepository &achievementRepository_;
    };
} // namespace cppforge::services
