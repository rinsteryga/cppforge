#pragma once

#include <cstdint>
#include <set>
#include <chrono>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class Achievement;
        
        class User
        {
        public:
            User(uint64_t id, const QString &username, const QString &email, const QString &passwordHash, const std::chrono::system_clock::time_point &createdAt);
            User(uint64_t id, const QString &username, const QString &email, const QString &passwordHash);

            uint64_t getId() const;
            QString getUsername() const;
            QString getEmail() const;
            QString getPasswordHash() const;

            QString getAvatarPath() const;
            QString getBio() const;
            std::chrono::system_clock::time_point getCreatedAt() const;

            std::set<uint64_t> getAvailableLevelsIds() const;
            std::set<uint64_t> getCompletedLevelsIds() const;
            std::set<uint64_t> getLockedLevelsIds() const;

            std::set<cppforge::entities::Achievement> getAchievements() const;

            void addAchievement(const cppforge::entities::Achievement &achievement);
            void removeAchievement(const cppforge::entities::Achievement &achievement);
            void clearAchievements();

            void setAvatarPath(const QString &path);
            void setBio(const QString &bio);

            void addAvailableLevel(uint64_t levelId);
            void addCompletedLevel(uint64_t levelId);
            void addLockedLevel(uint64_t levelId);

            void removeAvailableLevel(uint64_t levelId);
            void removeCompletedLevel(uint64_t levelId);
            void removeLockedLevel(uint64_t levelId);

        private:
            uint64_t id_;
            QString username_;
            QString email_;
            QString passwordHash_;

            QString avatarPath_;
            QString bio_;
            std::chrono::system_clock::time_point createdAt_;

            std::set<uint64_t> availableLevelsIds_;
            std::set<uint64_t> completedLevelsIds_;
            std::set<uint64_t> lockedLevelsIds_;

            std::set<cppforge::entities::Achievement> achievements_;
        };
    } // namespace entities
} // namespace cppforge
