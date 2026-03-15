#pragma once

#include "../entities/MatchingTask.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge::repositories
{
    /**
     * @brief Defines the abstract data access contract for MatchingTask entities.
     *
     * This interface handles the retrieval of interactive matching exercises,
     * connecting thematic pairs to their specific educational context.
     */
    class IMatchingTaskRepository
    {
    protected:
        IMatchingTaskRepository() = default;

    public:
        virtual ~IMatchingTaskRepository() = default;
        IMatchingTaskRepository(const IMatchingTaskRepository &) = delete;
        IMatchingTaskRepository &operator=(const IMatchingTaskRepository &) = delete;
        IMatchingTaskRepository(IMatchingTaskRepository &&) = delete;
        IMatchingTaskRepository &operator=(IMatchingTaskRepository &&) = delete;

        /**
         * @brief Retrieves all matching-type tasks tied to a specific lesson.
         *
         * @param lessonId The unique identifier of the lesson.
         * @return A vector of MatchingTask entities found for this lesson.
         */
        virtual std::vector<entities::MatchingTask> getMatchingTasksByLessonId(uint64_t lessonId) const = 0;

        /**
         * @brief Fetches a single matching task by its unique identifier.
         *
         * @param taskId The unique identifier of the matching task.
         * @return An std::optional containing the MatchingTask if found; otherwise std::nullopt.
         */
        virtual std::optional<entities::MatchingTask> getMatchingTaskById(uint64_t taskId) const = 0;
    };
} // namespace cppforge::repositories
