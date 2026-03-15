#pragma once

#include <QString>

#include <cstdint>

namespace cppforge
{
    namespace entities
    {
        class MatchingPair
        {
        public:
            MatchingPair(uint64_t id, const QString &leftItem, const QString &rightItem);

            uint64_t getId() const;
            const QString &getLeftItem() const;
            const QString &getRightItem() const;
            bool operator<(const MatchingPair &other) const;

        private:
            uint64_t id_;
            QString leftItem_;
            QString rightItem_;
        };
    } // namespace entities
} // namespace cppforge
