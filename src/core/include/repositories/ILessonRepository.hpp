#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge
{
    namespace entities
    {
        class Lesson;
    }

    namespace repositories
    {
        class ILessonRepository
        {
        public:
            virtual ~ILessonRepository() = default;

            virtual std::vector<entities::Lesson> getLessonsByModuleId(uint64_t moduleId) const = 0;
            virtual std::optional<entities::Lesson> getLessonById(uint64_t id) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
