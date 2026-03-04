#pragma once

#include <QString>

#include <cstdint>
#include <optional>
#include <set>

namespace cppforge
{
    namespace entities
    {
        class TestCase;

        class CodingTask
        {
        public:
            CodingTask(uint64_t id, uint64_t lessonId, const QString &title, const QString &description,
                       const QString &initialCode, const std::set<TestCase> &testCases, int32_t timeLimit,
                       int32_t memoryLimit, const std::optional<std::set<QString>> &whitelist = std::nullopt,
                       const std::optional<std::set<QString>> &blacklist = std::nullopt);

            uint64_t getId() const;
            uint64_t getLessonId() const;
            const QString &getTitle() const;
            const QString &getDescription() const;
            const QString &getInitialCode() const;
            const std::set<TestCase> &getTestCases() const;
            int32_t getTimeLimit() const;
            int32_t getMemoryLimit() const;
            const std::optional<std::set<QString>> &getWhitelist() const;
            const std::optional<std::set<QString>> &getBlacklist() const;

        private:
            uint64_t id_;
            uint64_t lessonId_;
            QString title_;
            QString description_;
            QString initialCode_;
            std::set<TestCase> testCases_;
            int32_t timeLimit_;
            int32_t memoryLimit_;
            std::optional<std::set<QString>> whitelist_;
            std::optional<std::set<QString>> blacklist_;
        };
    } // namespace entities
} // namespace cppforge
