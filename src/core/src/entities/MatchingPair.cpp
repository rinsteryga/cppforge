#include "../../include/entities/MatchingPair.hpp"

#include <utility>

namespace cppforge::entities
{
    MatchingPair::MatchingPair(uint64_t pairId, QString leftItem, QString rightItem)
        : id_(pairId), leftItem_(std::move(leftItem)), rightItem_(std::move(rightItem))
    {
    }

    uint64_t MatchingPair::getId() const
    {
        return id_;
    }

    const QString &MatchingPair::getLeftItem() const
    {
        return leftItem_;
    }

    const QString &MatchingPair::getRightItem() const
    {
        return rightItem_;
    }

    bool MatchingPair::operator<(const MatchingPair &other) const
    {
        return id_ < other.id_;
    }
} // namespace cppforge::entities
