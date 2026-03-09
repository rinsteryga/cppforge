#include "../../include/entities/CodingTask.hpp"

#include "../../include/entities/TestCase.hpp"

namespace cppforge::entities
{
    CodingTask::CodingTask(uint64_t id, uint64_t lessonId, const QString &title, const QString &description,
                           const QString &initialCode, const std::set<TestCase> &testCases, int32_t timeLimit,
                           int32_t memoryLimit, const std::optional<std::set<QString>> &whitelist,
                           const std::optional<std::set<QString>> &blacklist)
        : id_(id), lessonId_(lessonId), title_(title), description_(description), initialCode_(initialCode),
          testCases_(testCases), timeLimit_(timeLimit), memoryLimit_(memoryLimit), whitelist_(whitelist),
          blacklist_(blacklist)
    {
    }

    uint64_t CodingTask::getId() const
    {
        return id_;
    }

    uint64_t CodingTask::getLessonId() const
    {
        return lessonId_;
    }

    const QString &CodingTask::getTitle() const
    {
        return title_;
    }

    const QString &CodingTask::getDescription() const
    {
        return description_;
    }

    const QString &CodingTask::getInitialCode() const
    {
        return initialCode_;
    }

    const std::set<TestCase> &CodingTask::getTestCases() const
    {
        return testCases_;
    }

    int32_t CodingTask::getTimeLimit() const
    {
        return timeLimit_;
    }

    int32_t CodingTask::getMemoryLimit() const
    {
        return memoryLimit_;
    }

    const std::optional<std::set<QString>> &CodingTask::getWhitelist() const
    {
        return whitelist_;
    }

    const std::optional<std::set<QString>> &CodingTask::getBlacklist() const
    {
        return blacklist_;
    }
} // namespace cppforge::entities
