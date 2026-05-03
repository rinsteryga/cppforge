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

    std::optional<entities::User> UserService::findById(uint64_t userId) const
    {
        return userRepository_.findById(userId);
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

    void UserService::updateAvatar(uint64_t userId, const QString &avatarPath)
    {
        userRepository_.updateAvatar(userId, avatarPath);
    }

    bool UserService::saveLessonProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId, bool isCompleted)
    {
        return userRepository_.saveLessonProgress(userId, moduleId, lessonId, isCompleted);
    }

    bool UserService::saveSubmission(uint64_t userId, uint64_t moduleId, uint64_t taskId, const QString &code,
                                     bool isSuccess)
    {
        bool saved = userRepository_.saveSubmission(userId, moduleId, taskId, code, isSuccess);
        if (saved && isSuccess)
        {
            userRepository_.updateStreak(userId);
        }
        return saved;
    }

    int UserService::getTotalSubmissionsCount(uint64_t userId) const
    {
        return userRepository_.getTotalSubmissionsCount(userId);
    }

    bool UserService::recordDuelResult(uint64_t userId, bool isWin, std::optional<int> score)
    {
        auto userOpt = userRepository_.findById(userId);
        if (!userOpt)
        {
            return false;
        }

        auto &user = *userOpt;
        if (isWin)
        {
            user.setDuelWins(user.getDuelWins() + 1);
            int pointsToAdd = score.value_or(10);
            user.setDuelPoints(user.getDuelPoints() + pointsToAdd);
        }
        else
        {
            user.setDuelLosses(user.getDuelLosses() + 1);
            int pointsToSub = score.value_or(5);
            int newPoints = std::max(0, user.getDuelPoints() - pointsToSub);
            user.setDuelPoints(newPoints);
        }

        return userRepository_.save(user);
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
