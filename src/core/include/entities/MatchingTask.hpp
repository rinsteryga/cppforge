#pragma once

#include <QString>

#include <cstdint>
#include <set>

namespace cppforge::entities
{
    class MatchingPair;

    /**
     * @brief Represents a task where users must connect related items into pairs.
     *
     * A MatchingTask is an interactive exercise tied to a Lesson, typically
     * presenting a shuffled list of concepts and definitions that the user
     * must correctly associate.
     */
    class MatchingTask
    {
    public:
        MatchingTask() = default;

        /**
         * @brief Constructs a new MatchingTask instance.
         *
         * @param taskId The unique identifier of the task.
         * @param lessonId The ID of the parent lesson providing the context.
         * @param title The concise display title of the exercise.
         * @param description A full explanation of what the user needs to match.
         * @param pairs A complete set of correct MatchingPair items.
         */
        MatchingTask(uint64_t taskId, uint64_t lessonId, QString title, QString description,
                     const std::set<MatchingPair> &pairs);

        /**
         * @brief Retrieves the unique identifier of this matching exercise.
         * @return The 64-bit unsigned integer representing the task ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the parent lesson's identifier.
         * @return The 64-bit unsigned integer representing the lesson ID.
         */
        uint64_t lessonId() const;

        /**
         * @brief Retrieves the exercise's display title.
         * @return A constant reference to the QString containing the title.
         */
        const QString &getTitle() const;

        /**
         * @brief Retrieves the detailed instructional description.
         * @return A constant reference to the QString containing the description.
         */
        const QString &getDescription() const;

        /**
         * @brief Retrieves the set of all correct matching pairs answering the task.
         * @return A constant reference to the set of MatchingPair objects.
         */
        const std::set<MatchingPair> &getPairs() const;

    private:
        uint64_t id_;
        uint64_t lessonId_;
        QString title_;
        QString description_;
        std::set<MatchingPair> pairs_;
    };
} // namespace cppforge::entities

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::entities::MatchingTask)
