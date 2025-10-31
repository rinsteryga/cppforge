#pragma once

#include <cstdint>
#include <set>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class TestCase;

        class CodingTask
        {
        public:
            CodingTask(uint64_t id, uint64_t lessonId, const QString &title,
                       const QString &description, const QString &initialCode,
                       const std::set<TestCase> &testCases, int32_t timeLimit,
                       int32_t memoryLimit);

            uint64_t getId() const;
            uint64_t getLessonId() const;
            QString getTitle() const;
            QString getDescription() const;
            QString getInitialCode() const;
            std::set<TestCase> getTestCases() const;
            int32_t getTimeLimit() const;
            int32_t getMemoryLimit() const;

        private:
            uint64_t id_;
            uint64_t lessonId_;
            QString title_;
            QString description_;
            QString initialCode_;
            std::set<TestCase> testCases_;
            int32_t timeLimit_;
            int32_t memoryLimit_;
        };
    } // namespace entities
} // namespace cppforge
