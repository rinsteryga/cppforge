#include "../../include/entities/User.hpp"

#include "../../include/entities/Achievement.hpp"

#include <utility>

namespace cppforge::entities
{
    User::User(uint64_t userId, QString username, QString email, QString passwordHash, QString salt,
               const std::chrono::system_clock::time_point &createdAt)
        : id_(userId), username_(std::move(username)), email_(std::move(email)), passwordHash_(std::move(passwordHash)),
          salt_(std::move(salt)), currentStreakDays_(0), createdAt_(createdAt)
    {
    }

    User::User(uint64_t userId, QString username, QString email, QString passwordHash, QString salt)
        : id_(userId), username_(std::move(username)), email_(std::move(email)), passwordHash_(std::move(passwordHash)),
          salt_(std::move(salt)), currentStreakDays_(0), createdAt_(std::chrono::system_clock::now())
    {
    }

    uint64_t User::getId() const
    {
        return id_;
    }

    const QString &User::getUsername() const
    {
        return username_;
    }

    const QString &User::getEmail() const
    {
        return email_;
    }

    const QString &User::getPasswordHash() const
    {
        return passwordHash_;
    }

    const QString &User::getSalt() const
    {
        return salt_;
    }

    const QString &User::getAvatarPath() const
    {
        return avatarPath_;
    }

    const QString &User::getBio() const
    {
        return bio_;
    }

    const std::chrono::system_clock::time_point &User::getCreatedAt() const
    {
        return createdAt_;
    }

    uint32_t User::getCurrentStreakDays() const
    {
        return currentStreakDays_;
    }

    const std::chrono::system_clock::time_point &User::getLastLevelSolvedAt() const
    {
        return lastLevelSolvedAt_;
    }

    const std::set<uint64_t> &User::getAvailableLevelsIds() const
    {
        return availableLevelsIds_;
    }

    const std::set<uint64_t> &User::getCompletedLevelsIds() const
    {
        return completedLevelsIds_;
    }

    const std::set<uint64_t> &User::getLockedLevelsIds() const
    {
        return lockedLevelsIds_;
    }

    const std::set<cppforge::entities::Achievement> &User::getAchievements() const
    {
        return achievements_;
    }

    void User::addAchievement(const cppforge::entities::Achievement &achievement)
    {
        achievements_.insert(achievement);
    }

    void User::removeAchievement(const cppforge::entities::Achievement &achievement)
    {
        achievements_.erase(achievement);
    }

    void User::clearAchievements()
    {
        achievements_.clear();
    }

    void User::setAvatarPath(const QString &path)
    {
        avatarPath_ = path;
    }

    void User::setBio(const QString &bio)
    {
        bio_ = bio;
    }

    void User::setCurrentStreakDays(uint32_t streak)
    {
        currentStreakDays_ = streak;
    }

    void User::setLastLevelSolvedAt(const std::chrono::system_clock::time_point &timestamp)
    {
        lastLevelSolvedAt_ = timestamp;
    }

    void User::addAvailableLevel(uint64_t levelId)
    {
        availableLevelsIds_.insert(levelId);
    }

    void User::addCompletedLevel(uint64_t levelId)
    {
        completedLevelsIds_.insert(levelId);
    }

    void User::addLockedLevel(uint64_t levelId)
    {
        lockedLevelsIds_.insert(levelId);
    }

    void User::removeAvailableLevel(uint64_t levelId)
    {
        availableLevelsIds_.erase(levelId);
    }

    void User::removeCompletedLevel(uint64_t levelId)
    {
        completedLevelsIds_.erase(levelId);
    }

    void User::removeLockedLevel(uint64_t levelId)
    {
        lockedLevelsIds_.erase(levelId);
    }
} // namespace cppforge::entities
