#pragma once

#include <QString>

#include <chrono>
#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief The trigger type determining how an achievement is unlocked.
     */
    enum class ConditionType
    {
        LevelsCompleted,
        StreakDays,
        CustomEvent
    };

    /**
     * @brief Represents a user achievement within the application.
     *
     * Achievements are awarded to users upon completing specific milestones or tasks.
     * This class encapsulates the achievement's metadata, including its visual representation
     * and the exact time it was earned by the user.
     */
    class Achievement
    {
    public:
        Achievement() = default;
        
        /**
         * @brief Constructs a new Achievement instance.
         *
         * @param achievementId The unique identifier of the achievement.
         * @param name The display name of the achievement.
         * @param description A detailed explanation of what the achievement represents.
         * @param iconPath The file path or resource URI to the achievement's icon.
         * @param conditionType The type of condition required to unlock.
         * @param conditionValue The integer threshold value for the condition.
         * @param dateEarned The timestamp indicating when the user unlocked this achievement.
         */
        Achievement(int64_t achievementId, QString name, QString description, QString iconPath,
                    ConditionType conditionType, uint32_t conditionValue,
                    const std::chrono::system_clock::time_point &dateEarned);

        /**
         * @brief Retrieves the unique identifier of the achievement.
         * @return The 64-bit unsigned integer representing the ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the display name of the achievement.
         * @return A constant reference to the QString containing the name.
         */
        const QString &getName() const;

        /**
         * @brief Retrieves the detailed description of the achievement.
         * @return A constant reference to the QString containing the description.
         */
        const QString &getDescription() const;

        /**
         * @brief Retrieves the path to the achievement's icon.
         * @return A constant reference to the QString containing the icon path.
         */
        const QString &getIconPath() const;

        /**
         * @brief Retrieves the condition type of the achievement.
         * @return The condition required to unlock this achievement.
         */
        ConditionType getConditionType() const;

        /**
         * @brief Retrieves the numeric threshold for unlocking the achievement.
         * @return A 32-bit unsigned integer representing the condition value.
         */
        uint32_t getConditionValue() const;

        /**
         * @brief Retrieves the timestamp of when the achievement was earned.
         * @return A time_point representing the moment of unlocking.
         */
        std::chrono::system_clock::time_point getDateEarned() const;

        /**
         * @brief Compares this achievement with another for ordering.
         *
         * The comparison is primarily based on the achievement ID to allow sorting
         * within associative containers like std::set.
         *
         * @param other The other Achievement to compare against.
         * @return True if this achievement is considered less than the other; otherwise false.
         */
        bool operator<(const Achievement &other) const;

        /**
         * @brief Checks for equality between this achievement and another.
         *
         * Equality is evaluated based on the unique identifier.
         *
         * @param other The other Achievement to compare against.
         * @return True if both achievements share the same ID; otherwise false.
         */
        bool operator==(const Achievement &other) const;

    private:
        uint64_t id_;
        QString name_;
        QString description_;
        QString iconPath_;

        ConditionType conditionType_;
        uint32_t conditionValue_;

        std::chrono::system_clock::time_point dateEarned_;
    };
} // namespace cppforge::entities

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::entities::Achievement)
