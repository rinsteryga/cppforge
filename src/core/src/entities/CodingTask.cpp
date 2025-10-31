#include "../../include/entities/CodingTask.hpp"
#include "../../include/entities/TestCase.hpp"

namespace cppforge::entities
{
    CodingTask::CodingTask(uint64_t id, uint64_t lessonId, const QString &title,
                           const QString &description, const QString &initialCode,
                           const std::set<TestCase> &testCases, int32_t timeLimit,
                           int32_t memoryLimit)
        : id_(id), lessonId_(lessonId), title_(title), description_(description),
          initialCode_(initialCode), testCases_(testCases), timeLimit_(timeLimit),
          memoryLimit_(memoryLimit) {}

    uint64_t CodingTask::getId() const
    {
        return id_;
    }

    uint64_t CodingTask::getLessonId() const
    {
        return lessonId_;
    }

    QString CodingTask::getTitle() const
    {
        return title_;
    }

    QString CodingTask::getDescription() const
    {
        return description_;
    }

    QString CodingTask::getInitialCode() const
    {
        return initialCode_;
    }

    std::set<TestCase> CodingTask::getTestCases() const
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
}
