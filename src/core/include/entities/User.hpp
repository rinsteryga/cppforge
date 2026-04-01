#pragma once

#include "Achievement.hpp"

#include <QString>

#include <chrono>
#include <cstdint>
#include <set>

namespace cppforge::entities
{
    /**
     * @brief Represents a registered user of the cppforge platform.
     *
     * The User class is the central entity encapsulating personal profile data, authentication
     * credentials, and the user's relational associations like progress (unlocked levels)
     * and awarded achievements.
     */
    class User
    {
    public:
        /**
         * @brief Constructs a new User instance with registration time.
         *
         * @param userId The unique identifier of the user account.
         * @param username The display name of the user.
         * @param email The registered email address for login and contact.
         * @param passwordHash The securely hashed password snippet.
         * @param createdAt The exact timestamp of account creation.
         */
        User(uint64_t userId, QString username, QString email, QString passwordHash,
             const std::chrono::system_clock::time_point &createdAt);

        /**
         * @brief Constructs a new User instance without explicitly defining creation time.
         *
         * @param userId The unique identifier of the user account.
         * @param username The display name of the user.
         * @param email The registered email address.
         * @param passwordHash The securely hashed password snippet.
         */
        User(uint64_t userId, QString username, QString email, QString passwordHash);

        /**
         * @brief Retrieves the user's central unique identifier.
         * @return The 64-bit unsigned integer representing the profile ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the user's chosen display name.
         * @return A constant reference to the username string.
         */
        const QString &getUsername() const;

        /**
         * @brief Retrieves the user's registered email address.
         * @return A constant reference to the email string.
         */
        const QString &getEmail() const;

        /**
         * @brief Retrieves the user's password cipher.
         * @return A constant reference to the password hash string.
         */
        const QString &getPasswordHash() const;

        /**
         * @brief Retrieves the relative or absolute path to the user's graphical avatar.
         * @return A constant reference to the avatar path string.
         */
        const QString &getAvatarPath() const;

        /**
         * @brief Retrieves the personalized biographical string of the user.
         * @return A constant reference to the biography string.
         */
        const QString &getBio() const;

        /**
         * @brief Retrieves the timestamp when the profile was generated.
         * @return A time_point reflecting the creation date.
         */
        const std::chrono::system_clock::time_point &getCreatedAt() const;

        /**
         * @brief Retrieves the user's current consecutive days of solving tasks.
         * @return A 32-bit unsigned integer representing the streak.
         */
        uint32_t getCurrentStreakDays() const;

        /**
         * @brief Retrieves the timestamp of when the user last solved a task.
         * @return A time_point reflecting the last solved date.
         */
        const std::chrono::system_clock::time_point &getLastLevelSolvedAt() const;

        /**
         * @brief Retrieves the set of level IDs currently open for the user to solve.
         * @return A constant reference to a generic set containing accessible level IDs.
         */
        const std::set<uint64_t> &getAvailableLevelsIds() const;

        /**
         * @brief Retrieves the set of level IDs the user has fully resolved.
         * @return A constant reference to a generic set containing finished level IDs.
         */
        const std::set<uint64_t> &getCompletedLevelsIds() const;

        /**
         * @brief Retrieves the set of level IDs currently inaccessible to the user.
         * @return A constant reference to a generic set containing restricted level IDs.
         */
        const std::set<uint64_t> &getLockedLevelsIds() const;

        /**
         * @brief Retrieves the comprehensive list of badges or milestones passed by the user.
         * @return A constant reference to a set of Achievement entities.
         */
        const std::set<cppforge::entities::Achievement> &getAchievements() const;

        /**
         * @brief Grants a new achievement to the user's profile.
         * @param achievement The Achievement object to append.
         */
        void addAchievement(const cppforge::entities::Achievement &achievement);

        /**
         * @brief Revokes a previously granted achievement from the user's profile.
         * @param achievement The Achievement object to remove.
         */
        void removeAchievement(const cppforge::entities::Achievement &achievement);

        /**
         * @brief Deletes all collected achievements tied to the user.
         */
        void clearAchievements();

        /**
         * @brief Updates the user's custom avatar directory link.
         * @param path The new valid path string to the targeted image.
         */
        void setAvatarPath(const QString &path);

        /**
         * @brief Overwrites the user's biographical description.
         * @param bio The newly structured contextual text snippet.
         */
        void setBio(const QString &bio);

        /**
         * @brief Updates the user's current consecutive days streak.
         * @param streak The new streak count.
         */
        void setCurrentStreakDays(uint32_t streak);

        /**
         * @brief Updates the timestamp of when the user last solved a task.
         * @param timestamp The exact time to set.
         */
        void setLastLevelSolvedAt(const std::chrono::system_clock::time_point &timestamp);

        /**
         * @brief Unlocks a target level, allowing the user to attempt it.
         * @param levelId The ID indicating the specified scope level.
         */
        void addAvailableLevel(uint64_t levelId);

        /**
         * @brief Marks a target level as successfully bridged or overcome.
         * @param levelId The ID indicating the specified scope level.
         */
        void addCompletedLevel(uint64_t levelId);

        /**
         * @brief Pushes a target level into the restricted boundaries.
         * @param levelId The ID indicating the specified scope level.
         */
        void addLockedLevel(uint64_t levelId);

        /**
         * @brief Revokes the authorization to attempt a prior target level.
         * @param levelId The ID indicating the specified scope level.
         */
        void removeAvailableLevel(uint64_t levelId);

        /**
         * @brief Demotes a targeted level from the completed segment.
         * @param levelId The ID indicating the specified scope level.
         */
        void removeCompletedLevel(uint64_t levelId);

        /**
         * @brief Promotes a targeted level out of restricted boundaries.
         * @param levelId The ID indicating the specified scope level.
         */
        void removeLockedLevel(uint64_t levelId);

    private:
        uint64_t id_;
        QString username_;
        QString email_;
        QString passwordHash_;

        QString avatarPath_;
        QString bio_;
        uint32_t currentStreakDays_;
        std::chrono::system_clock::time_point lastLevelSolvedAt_;
        std::chrono::system_clock::time_point createdAt_;

        std::set<uint64_t> availableLevelsIds_;
        std::set<uint64_t> completedLevelsIds_;
        std::set<uint64_t> lockedLevelsIds_;

        std::set<cppforge::entities::Achievement> achievements_;
    };
} // namespace cppforge::entities
