#include "../../include/entities/MatchingTask.hpp"

#include "../../include/entities/MatchingPair.hpp"

#include <utility>

namespace cppforge::entities
{
    MatchingTask::MatchingTask(uint64_t taskId, uint64_t lessonId, QString title, QString description,
                               const std::set<MatchingPair> &pairs)
        : id_(taskId), lessonId_(lessonId), title_(std::move(title)), description_(std::move(description)), pairs_(pairs)
    {
    }

    uint64_t MatchingTask::getId() const
    {
        return id_;
    }

    uint64_t MatchingTask::lessonId() const
    {
        return lessonId_;
    }

    const QString &MatchingTask::getTitle() const
    {
        return title_;
    }

    const QString &MatchingTask::getDescription() const
    {
        return description_;
    }

    const std::set<MatchingPair> &MatchingTask::getPairs() const
    {
        return pairs_;
    }
} // namespace cppforge::entities
