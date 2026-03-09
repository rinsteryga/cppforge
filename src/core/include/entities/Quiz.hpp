#pragma once

#include <QString>

#include <cstdint>
#include <set>

namespace cppforge
{
    namespace entities
    {
        class QuizOption;

        class Quiz
        {
        public:
            Quiz(uint64_t id, uint64_t lessonId, const QString &title, const QString &question,
                 const std::set<QuizOption> &options);

            uint64_t getId() const;
            uint64_t getLessonId() const;
            const QString &getTitle() const;
            const QString &getQuestion() const;
            const std::set<QuizOption> &getOptions() const;

        private:
            uint64_t id_;
            uint64_t lessonId_;
            QString title_;
            QString question_;
            std::set<QuizOption> options_;
        };
    } // namespace entities
} // namespace cppforge
