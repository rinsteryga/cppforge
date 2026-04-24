#include "../../include/services/AchievementService.hpp"

#include "../../include/entities/Achievement.hpp"

#include <algorithm>
#include <functional>
#include <unordered_map>

namespace cppforge::services
{
    AchievementService::AchievementService(repositories::IUserRepository &userRepo,
                                           repositories::IAchievementRepository &achievementRepo)
        : QObject(nullptr), userRepo_(userRepo), achievementRepo_(achievementRepo)
    {
    }

    void AchievementService::checkAndAwardAchievements(entities::User &user)
    {
        auto systemAchievements = achievementRepo_.findAll();
        const auto &userAchievements = user.getAchievements();
        bool userUpdated = false;

        using ValidatorFn = std::function<bool(const entities::User &, uint32_t)>;
        std::unordered_map<entities::ConditionType, ValidatorFn> validators = {
            {entities::ConditionType::LevelsCompleted,
             [](const entities::User &user, uint32_t val) { return user.getCompletedLevelsIds().size() >= val; }},
            {entities::ConditionType::StreakDays,
             [](const entities::User &user, uint32_t val) { return user.getCurrentStreakDays() >= val; }},
            {entities::ConditionType::CustomEvent, [](const entities::User &, uint32_t) { return false; }}};

        for (const auto &achievement : systemAchievements)
        {
            auto iter = std::find_if(userAchievements.begin(), userAchievements.end(),
                                     [&](const entities::Achievement &userAchievement)
                                     { return userAchievement.getId() == achievement.getId(); });

            if (iter != userAchievements.end())
            {
                continue;
            }

            auto valIt = validators.find(achievement.getConditionType());
            bool unlocked = (valIt != validators.end()) ? valIt->second(user, achievement.getConditionValue()) : false;

            if (unlocked)
            {
                entities::Achievement newAchievement(static_cast<int64_t>(achievement.getId()), achievement.getName(),
                                                     achievement.getDescription(), achievement.getIconPath(),
                                                     achievement.getConditionType(), achievement.getConditionValue(),
                                                     std::chrono::system_clock::now());

                user.addAchievement(newAchievement);
                emit achievementUnlocked(newAchievement);
                userUpdated = true;
            }
        }

        if (userUpdated)
        {
            userRepo_.save(user);
        }
    }

    void AchievementService::awardCustomEvent(entities::User &user, const QString &customEventName)
    {
        auto systemAchievements = achievementRepo_.findAll();
        const auto &userAchievements = user.getAchievements();

        auto achIt =
            std::find_if(systemAchievements.begin(), systemAchievements.end(),
                         [&](const entities::Achievement &systemAchievement)
                         {
                             return systemAchievement.getName() == customEventName &&
                                    systemAchievement.getConditionType() == entities::ConditionType::CustomEvent;
                         });

        if (achIt != systemAchievements.end())
        {
            auto hasIt = std::find_if(userAchievements.begin(), userAchievements.end(),
                                      [&](const entities::Achievement &userAchievement)
                                      { return userAchievement.getId() == achIt->getId(); });

            if (hasIt == userAchievements.end())
            {
                entities::Achievement newAchievement(static_cast<int64_t>(achIt->getId()), achIt->getName(),
                                                     achIt->getDescription(), achIt->getIconPath(),
                                                     achIt->getConditionType(), achIt->getConditionValue(),
                                                     std::chrono::system_clock::now());

                user.addAchievement(newAchievement);
                emit achievementUnlocked(newAchievement);
                userRepo_.save(user);
            }
        }
    }
} // namespace cppforge::services
