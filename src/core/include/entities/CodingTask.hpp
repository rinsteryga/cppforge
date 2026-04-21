#pragma once

#include "TestCase.hpp"

#include <QString>

#include <cstdint>
#include <optional>
#include <set>

namespace cppforge::entities
{
    /**
     * @brief Represents a coding exercise assigned to a specific lesson.
     *
     * A CodingTask contains the requirements, initial code template, and the evaluation criteria
     * (test cases) necessary for a user to solve a programming problem. It also specifies the
     * execution constraints such as time and memory limits, alongside optional lists of
     * required or forbidden language constructs (whitelists and blacklists).
     */
    class CodingTask
    {
    public:
        CodingTask();
        /**
         * @brief Constructs a new CodingTask instance.
         *
         * @param taskId The unique identifier of the coding task.
         * @param lessonId The optional ID of the lesson to which this task belongs.
         * @param title The concise title of the coding problem.
         * @param description The detailed problem statement and instructions.
         * @param initialCode The starting code snippet provided to the user.
         * @param testCases A set of test cases used to evaluate the user's submission.
         * @param timeLimit The maximum allowed execution time in milliseconds.
         * @param memoryLimit The maximum allowed memory usage in kilobytes.
         * @param whitelist An optional set of syntax tokens that must be present in the solution.
         * @param blacklist An optional set of syntax tokens that must not be used in the solution.
         * @param duelTopic An optional topic for duel tasks.
         */
        CodingTask(uint64_t taskId, std::optional<uint64_t> lessonId, QString title, QString description, QString initialCode,
                   const std::set<TestCase> &testCases, int32_t timeLimit, int32_t memoryLimit,
                   const std::optional<std::set<QString>> &whitelist = std::nullopt,
                   const std::optional<std::set<QString>> &blacklist = std::nullopt,
                   const std::optional<QString> &duelTopic = std::nullopt);

        /**
         * @brief Retrieves the unique identifier of the coding task.
         * @return The task ID as a 64-bit unsigned integer.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the parent lesson's identifier.
         * @return An optional containing the lesson ID as a 64-bit unsigned integer.
         */
        std::optional<uint64_t> getLessonId() const;

        /**
         * @brief Retrieves the title of the coding task.
         * @return A constant reference to the QString containing the title.
         */
        const QString &getTitle() const;

        /**
         * @brief Retrieves the full descriptive text of the task.
         * @return A constant reference to the QString containing the description.
         */
        const QString &getDescription() const;

        /**
         * @brief Retrieves the initial code block provided to the user.
         * @return A constant reference to the initial code snippet.
         */
        const QString &getInitialCode() const;

        /**
         * @brief Retrieves the collection of test cases used for evaluation.
         * @return A constant reference to a generic set containing all test cases.
         */
        const std::set<TestCase> &getTestCases() const;

        /**
         * @brief Retrieves the maximum allowed execution time for the task's solution.
         * @return The time limit in milliseconds.
         */
        int32_t getTimeLimit() const;

        /**
         * @brief Retrieves the maximum allowed memory consumption for the task's solution.
         * @return The memory limit in kilobytes.
         */
        int32_t getMemoryLimit() const;

        /**
         * @brief Retrieves the optional list of mandatory tokens for the task.
         * @return An optional set of whitelisted string tokens.
         */
        const std::optional<std::set<QString>> &getWhitelist() const;

        /**
         * @brief Retrieves the optional list of forbidden tokens for the task.
         * @return An optional set of blacklisted string tokens.
         */
        const std::optional<std::set<QString>> &getBlacklist() const;

        /**
         * @brief Retrieves the optional duel topic for this task.
         * @return An optional string representing the duel topic.
         */
        const std::optional<QString> &getDuelTopic() const;

    private:
        uint64_t id_;
        std::optional<uint64_t> lessonId_;
        QString title_;
        QString description_;
        QString initialCode_;
        std::set<TestCase> testCases_;
        int32_t timeLimit_;
        int32_t memoryLimit_;
        std::optional<std::set<QString>> whitelist_;
        std::optional<std::set<QString>> blacklist_;
        std::optional<QString> duelTopic_;
    };
} // namespace cppforge::entities

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::entities::CodingTask)
