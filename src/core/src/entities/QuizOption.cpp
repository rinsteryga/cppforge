#include "../../include/entities/QuizOption.hpp"

#include <utility>

namespace cppforge::entities
{
    QuizOption::QuizOption(uint64_t optionId, QString optionText, bool isCorrect)
        : id_(optionId), optionText_(std::move(optionText)), isCorrect_(isCorrect)
    {
    }

    uint64_t QuizOption::getId() const
    {
        return id_;
    }

    const QString &QuizOption::getOptionText() const
    {
        return optionText_;
    }

    bool QuizOption::isCorrect() const
    {
        return isCorrect_;
    }

    bool QuizOption::operator<(const QuizOption &other) const
    {
        return id_ < other.id_;
    }
} // namespace cppforge::entities
