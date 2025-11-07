#pragma once

#include <cstdint>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class ExecutionResult;

        class UserProgress
        {
        public:
            UserProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId,
                         bool isCompleted);

            uint64_t getUserId() const;
            uint64_t getModuleId() const;
            uint64_t getLessonId() const;
            bool isCompleted() const;

            void setCompleted(bool completed);
            
        private:
            uint64_t userId_;
            uint64_t moduleId_;
            uint64_t lessonId_;
            bool isCompleted_;
        };
    } // namespace entities
} // namespace cppforge
