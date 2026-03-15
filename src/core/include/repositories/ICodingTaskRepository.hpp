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
        class ICodingTaskRepository
        {
        public:
            virtual ~ICodingTaskRepository() = default;

            virtual std::vector<entities::CodingTask> getTasksByLessonId(uint64_t lessonId) const = 0;
            virtual std::optional<entities::CodingTask> getTaskById(uint64_t id) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
