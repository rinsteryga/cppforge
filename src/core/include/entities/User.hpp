#pragma once

#include "Achievement.hpp"

#include <QString>

#include <chrono>
#include <cstdint>
#include <set>

namespace cppforge
{
    namespace entities
    {
        class User
        {
        public:
            User(uint64_t id, const QString &username, const QString &email, const QString &passwordHash,
                 const std::chrono::system_clock::time_point &createdAt);
            User(uint64_t id, const QString &username, const QString &email, const QString &passwordHash);

            uint64_t getId() const;
            const QString &getUsername() const;
            const QString &getEmail() const;
            const QString &getPasswordHash() const;

            const QString &getAvatarPath() const;
            const QString &getBio() const;
            const std::chrono::system_clock::time_point &getCreatedAt() const;

            const std::set<uint64_t> &getAvailableLevelsIds() const;
            const std::set<uint64_t> &getCompletedLevelsIds() const;
            const std::set<uint64_t> &getLockedLevelsIds() const;

            const std::set<cppforge::entities::Achievement> &getAchievements() const;

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
