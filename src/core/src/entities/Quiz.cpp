#include "../../include/entities/Quiz.hpp"

#include "../../include/entities/QuizOption.hpp"

#include <utility>

namespace cppforge::entities
{
    Quiz::Quiz(uint64_t quizId, uint64_t lessonId, QString title, QString question, const std::set<QuizOption> &options)
        : id_(quizId), lessonId_(lessonId), title_(std::move(title)), question_(std::move(question)), options_(options)
    {
    }

    uint64_t Quiz::getId() const
    {
        return id_;
    }

    uint64_t Quiz::getLessonId() const
    {
        return lessonId_;
    }

    const QString &Quiz::getTitle() const
    {
        return title_;
    }

    const QString &Quiz::getQuestion() const
    {
        return question_;
    }

    const std::set<QuizOption> &Quiz::getOptions() const
    {
        return options_;
    }
} // namespace cppforge::entities
