#pragma once

#include "../entities/RoadmapNodeData.hpp"
#include "../entities/TaskWindowData.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge::services
{
    /**
     * @brief Service providing high-level operations for modules, lessons, and course progression.
     *
     * CourseService abstracts the data access logic required to build course roadmaps
     * and calculate user progress across different modules. It serves as the primary
     * business logic layer for course-related data.
     */
    class CourseService
    {
    public:
        /**
         * @brief Constructs a new CourseService instance.
         */
        CourseService() = default;

        /**
         * @brief Retrieves the sequence of lessons for a specific module as a roadmap.
         *
         * This method calculates the progression state (completed, locked) for each lesson
         * within the module for the specified user. It abstracts away the raw SQL queries
         * previously found in the GUI layer.
         *
         * @param userId The unique identifier of the user.
         * @param moduleId The unique identifier of the module.
         * @return A vector of RoadmapNodeData representing the sequential lessons and their states.
         */
        std::vector<entities::RoadmapNodeData> getModuleRoadmap(uint64_t userId, uint64_t moduleId) const;

        /**
         * @brief Retrieves the completion progress percentage for all available modules.
         *
         * @param userId The unique identifier of the user.
         * @return A vector of integers, where each element represents the completion percentage (0-100) of a module.
         */
        std::vector<int> getAllModulesProgress(uint64_t userId) const;

        /**
         * @brief Retrieves the completion progress percentage for a specific module.
         *
         * @param userId The unique identifier of the user.
         * @param moduleId The unique identifier of the module.
         * @return The completion percentage (0-100) of the module.
         */
        int getModuleProgress(uint64_t userId, uint64_t moduleId) const;

        /**
         * @brief Fetches all data required to render the TaskWindow for a specific lesson.
         *
         * @param userId The unique identifier of the user.
         * @param lessonId The unique identifier of the lesson.
         * @return An optional containing the TaskWindowData if found.
         */
        std::optional<entities::TaskWindowData> getTaskWindowData(uint64_t userId, uint64_t lessonId) const;

        /**
         * @brief Gets the ID of the chronologically next lesson after the given one.
         *
         * @param currentLessonId The unique identifier of the current lesson.
         * @return An optional containing the next lesson ID if available.
         */
        std::optional<uint64_t> getNextLessonId(uint64_t currentLessonId) const;

        /**
         * @brief Gets the ID of the chronologically previous lesson before the given one.
         *
         * @param currentLessonId The unique identifier of the current lesson.
         * @return An optional containing the previous lesson ID if available.
         */
        std::optional<uint64_t> getPrevLessonId(uint64_t currentLessonId) const;
    };
} // namespace cppforge::services
