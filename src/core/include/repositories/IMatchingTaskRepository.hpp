#pragma once

#include "../entities/MatchingTask.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge
{
    namespace repositories
    {
        class IMatchingTaskRepository
        {
        public:
            virtual ~IMatchingTaskRepository() = default;

            virtual std::vector<entities::MatchingTask> getMatchingTasksByLessonId(uint64_t lessonId) const = 0;
            virtual std::optional<entities::MatchingTask> getMatchingTaskById(uint64_t id) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
