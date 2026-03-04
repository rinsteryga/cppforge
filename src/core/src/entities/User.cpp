#include "../../include/entities/User.hpp"

#include "../../include/entities/Achievement.hpp"

namespace cppforge::entities
{
    User::User(uint64_t id, const QString &username, const QString &email, const QString &passwordHash,
               const std::chrono::system_clock::time_point &createdAt)
        : id_(id), username_(username), email_(email), passwordHash_(passwordHash), createdAt_(createdAt)
    {
    }

    User::User(uint64_t id, const QString &username, const QString &email, const QString &passwordHash)
        : id_(id), username_(username), email_(email), passwordHash_(passwordHash),
          createdAt_(std::chrono::system_clock::now())
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
