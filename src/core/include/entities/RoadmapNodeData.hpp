#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief Represents the logical state of a lesson node within a module's roadmap.
     *
     * This data structure holds the business logic representation of a roadmap node,
     * abstracting away any GUI-specific details (such as positioning). It encapsulates
     * the core properties necessary to determine a lesson's progression state.
     */
    struct RoadmapNodeData
    {
        /**
         * @brief The unique identifier of the lesson.
         */
        uint64_t lessonId;

        /**
         * @brief The display title of the lesson.
         */
        QString title;

        /**
         * @brief Indicates whether the user has successfully completed this lesson.
         */
        bool isCompleted;

        /**
         * @brief Indicates whether this lesson is currently locked for the user.
         *
         * A lesson typically becomes unlocked when the preceding lessons in the
         * module have been completed.
         */
        bool isLocked;
    };
} // namespace cppforge::entities
