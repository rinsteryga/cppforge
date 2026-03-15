#include "../../include/entities/MatchingTask.hpp"

#include "../../include/entities/MatchingPair.hpp"

namespace cppforge::entities
{
    MatchingTask::MatchingTask(uint64_t id, uint64_t lessonId, const QString &title, const QString &description,
                               const std::set<MatchingPair> &pairs)
        : id_(id), lessonId_(lessonId), title_(title), description_(description), pairs_(pairs)
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
