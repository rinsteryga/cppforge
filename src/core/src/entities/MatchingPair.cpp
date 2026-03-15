#include "../../include/entities/MatchingPair.hpp"

namespace cppforge::entities
{
    MatchingPair::MatchingPair(uint64_t id, const QString &leftItem, const QString &rightItem)
        : id_(id), leftItem_(leftItem), rightItem_(rightItem)
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
