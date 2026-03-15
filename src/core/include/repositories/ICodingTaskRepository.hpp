#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge
{
    namespace entities
    {
        class CodingTask;
    }

    namespace repositories
    {
        /**
         * @brief Defines the abstract data access contract for CodingTask entities.
         *
         * This interface dictates how the application must fetch CodingTask instances
         * from the underlying data store, decoupling business logic from database tech.
         */
        class ICodingTaskRepository
        {
        protected:
            ICodingTaskRepository() = default;

        public:
            virtual ~ICodingTaskRepository() = default;
            ICodingTaskRepository(const ICodingTaskRepository &) = delete;
            ICodingTaskRepository &operator=(const ICodingTaskRepository &) = delete;
            ICodingTaskRepository(ICodingTaskRepository &&) = delete;
            ICodingTaskRepository &operator=(ICodingTaskRepository &&) = delete;

            /**
             * @brief Retrieves all coding tasks associated with a specific lesson.
             *
             * @param lessonId The unique identifier of the parent lesson.
             * @return A vector of CodingTask entities belonging to the lesson.
             */
            virtual std::vector<entities::CodingTask> getTasksByLessonId(uint64_t lessonId) const = 0;

            /**
             * @brief Fetches a single coding task by its unique identifier.
             *
             * @param taskId The unique identifier of the coding task.
             * @return An std::optional containing the CodingTask if found; otherwise std::nullopt.
             */
            virtual std::optional<entities::CodingTask> getTaskById(uint64_t taskId) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
