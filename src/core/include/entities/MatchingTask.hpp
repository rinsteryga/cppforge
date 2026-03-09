#pragma once

#include <QString>

#include <cstdint>
#include <set>

namespace cppforge
{
    namespace entities
    {
        class MatchingPair;

        class MatchingTask
        {
        public:
            MatchingTask(uint64_t id, uint64_t lessonId, const QString &title, const QString &description,
                         const std::set<MatchingPair> &pairs);

            uint64_t getId() const;
            uint64_t lessonId() const;
            const QString &getTitle() const;
            const QString &getDescription() const;
            const std::set<MatchingPair> &getPairs() const;

        private:
            uint64_t id_;
            uint64_t lessonId_;
            QString title_;
            QString description_;
            std::set<MatchingPair> pairs_;
        };
    } // namespace entities
} // namespace cppforge
