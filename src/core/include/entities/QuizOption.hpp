#pragma once

#include <QString>

#include <cstdint>

namespace cppforge
{
    namespace entities
    {
        class QuizOption
        {
        public:
            QuizOption(uint64_t id, const QString &optionText, bool isCorrect);

            uint64_t getId() const;
            const QString &getOptionText() const;
            bool isCorrect() const;
            bool operator<(const QuizOption &other) const;

        private:
            uint64_t id_;
            QString optionText_;
            bool isCorrect_;
        };
    } // namespace entities
} // namespace cppforge
