#pragma once

#include "TestCase.hpp"

#include <QString>

#include <cstdint>
#include <optional>
#include <set>

namespace cppforge::entities
{
    /**
     * @brief A Data Transfer Object representing the full state required by the TaskWindow.
     *
     * This entity encapsulates the theory content, practical coding task details, and
     * the user's specific progress state for a single lesson.
     */
    struct TaskWindowData
    {
        /** @brief The display title of the lesson. */
        QString title;

        /** @brief The markdown or HTML theory content for the lesson. */
        QString theoryContent;

        /** @brief The ID of the module this lesson belongs to. */
        uint64_t moduleId;

        /** @brief Indicates whether the user has successfully completed this lesson. */
        bool isCompleted;

        /** @brief (Optional) The ID of the coding task, if the lesson contains one. */
        std::optional<uint64_t> taskId;

        /** @brief (Optional) The instructions/description for the coding task. */
        std::optional<QString> practiceDescription;

        /** @brief (Optional) The initial code template provided to the user. */
        std::optional<QString> initialCode;

        /** @brief (Optional) The most recent code submitted by the user. */
        std::optional<QString> previousCode;

        /** @brief The collection of test cases used to validate the coding task. */
        std::set<TestCase> testCases;
    };
} // namespace cppforge::entities
