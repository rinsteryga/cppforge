#include "../../include/services/UserService.hpp"

#include <algorithm>

namespace cppforge::services
{
    UserService::UserService(repositories::IUserRepository &userRepository,
                             repositories::IAchievementRepository &achievementRepository)
        : userRepository_(userRepository), achievementRepository_(achievementRepository)
    {
    }

    std::optional<entities::User> UserService::getUser(const QString &username) const
    {
        return userRepository_.findByUsername(username);
    }

    int UserService::getSolvedTasksCount(uint64_t userId) const
    {
        return userRepository_.getSolvedTasksCount(userId);
    }

    int UserService::getCompletedLessonsCount(uint64_t userId) const
    {
        return userRepository_.getCompletedLessonsCount(userId);
    }

    int UserService::getAchievementsCount(uint64_t userId) const
    {
        return userRepository_.getAchievementsCount(userId);
    }

    int UserService::getStreak(uint64_t userId) const
    {
        return userRepository_.getStreak(userId);
    }

    int UserService::getTotalSubmissionsCount(uint64_t userId) const
    {
        return userRepository_.getTotalSubmissionsCount(userId);
    }

    std::vector<UserService::Activity> UserService::getRecentActivity(uint64_t userId, int limit) const
    {
        return userRepository_.getRecentActivity(userId, limit);
    }

    std::vector<entities::Achievement> UserService::getAllAchievementsStatus(uint64_t userId) const
    {
        auto all = achievementRepository_.findAll();
        auto earnedIds = userRepository_.getEarnedAchievementIds(userId);

        for (auto &ach : all)
        {
            auto iter = std::find(earnedIds.begin(), earnedIds.end(), ach.getId());
            if (iter != earnedIds.end())
            {
                ach.setDateEarned(std::chrono::system_clock::now());
            }
        }
        return all;
    }
} // namespace cppforge::services
