#pragma once

#include "../entities/Quiz.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace cppforge
{
    namespace repositories
    {
        class IQuizRepository
        {
        public:
            virtual ~IQuizRepository() = default;

            virtual std::vector<entities::Quiz> getQuizzesByLessonId(uint64_t lessonId) const = 0;
            virtual std::optional<entities::Quiz> getQuizById(uint64_t id) const = 0;
        };
    } // namespace repositories
} // namespace cppforge
