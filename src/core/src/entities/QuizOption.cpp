#include "../../include/entities/QuizOption.hpp"

namespace cppforge::entities
{
    QuizOption::QuizOption(uint64_t id, const QString &optionText, bool isCorrect)
        : id_(id), optionText_(optionText), isCorrect_(isCorrect)
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
