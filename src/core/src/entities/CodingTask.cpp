#include "../../include/entities/CodingTask.hpp"

#include "../../include/entities/TestCase.hpp"

#include <utility>

namespace cppforge::entities
{
    CodingTask::CodingTask() = default;

    CodingTask::CodingTask(uint64_t taskId, std::optional<uint64_t> lessonId, QString title, QString description, QString initialCode,
                           const std::set<TestCase> &testCases, int32_t timeLimit, int32_t memoryLimit,
                           const std::optional<std::set<QString>> &whitelist,
                           const std::optional<std::set<QString>> &blacklist,
                           const std::optional<QString> &duelTopic)
        : id_(taskId), lessonId_(lessonId), title_(std::move(title)), description_(std::move(description)),
          initialCode_(std::move(initialCode)), testCases_(testCases), timeLimit_(timeLimit), memoryLimit_(memoryLimit),
          whitelist_(whitelist), blacklist_(blacklist), duelTopic_(duelTopic)
    {
    }

    uint64_t CodingTask::getId() const
    {
        return id_;
    }

    std::optional<uint64_t> CodingTask::getLessonId() const
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

    const std::optional<QString> &CodingTask::getDuelTopic() const
    {
        return duelTopic_;
    }
} // namespace cppforge::entities
