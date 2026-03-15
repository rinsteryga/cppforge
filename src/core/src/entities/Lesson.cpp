#include "../../include/entities/Lesson.hpp"

#include <utility>

namespace cppforge::entities
{
    Lesson::Lesson(uint64_t lessonId, uint64_t moduleId, QString title, QString content)
        : id_(lessonId), moduleId_(moduleId), title_(std::move(title)), content_(std::move(content))
    {
    }

    uint64_t Lesson::getId() const
    {
        return id_;
    }

    uint64_t Lesson::getModuleId() const
    {
        return moduleId_;
    }

    const QString &Lesson::getTitle() const
    {
        return title_;
    }

    const QString &Lesson::getContent() const
    {
        return content_;
    }
} // namespace cppforge::entities
