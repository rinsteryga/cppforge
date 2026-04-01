#pragma once

#include "../entities/User.hpp"
#include "../repositories/IAchievementRepository.hpp"
#include "../repositories/IUserRepository.hpp"

namespace cppforge::services
{
    /**
     * @brief Service responsible for evaluating and granting user achievements.
     *
     * This service encapsulates the business logic for determining if a user
     * has met the criteria for specific achievements based on their progress
     * and streak. It leverages repositories to persist these state changes.
     */
    class AchievementService
    {
    public:
        /**
         * @brief Constructs the service with required repositories.
         * @param userRepo Reference to the repository handling user persistence.
         * @param achievementRepo Reference to the repository containing available achievements.
         */
        AchievementService(repositories::IUserRepository &userRepo,
                           repositories::IAchievementRepository &achievementRepo);

        /**
         * @brief Evaluates a user's progress and grants newly unlocked achievements.
         *
         * Checks the user's completed levels and streak against the conditions
         * for all system achievements not yet earned, and persists the changes.
         *
         * @param user The user entity to evaluate and update.
         */
        void checkAndAwardAchievements(entities::User &user);

        /**
         * @brief Grants a specific custom achievement by name manually.
         *
         * Useful for event-driven logic (e.g. Easter eggs, finding hidden buttons).
         *
         * @param user The user entity to evaluate and update.
         * @param customEventName The exact name of the achievement to grant.
         */
        void awardCustomEvent(entities::User &user, const QString &customEventName);

    private:
        repositories::IUserRepository &userRepo_;
        repositories::IAchievementRepository &achievementRepo_;
    };
} // namespace cppforge::services
