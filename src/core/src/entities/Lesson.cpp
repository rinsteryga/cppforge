#include "../../include/entities/Lesson.hpp"

namespace cppforge::entities
{
    Lesson::Lesson(uint64_t id, uint64_t moduleId, const QString &title, const QString &content)
        : id_(id), moduleId_(moduleId), title_(title), content_(content) {}

    uint64_t Lesson::getId() const
    {
        return id_;
    }

    uint64_t Lesson::getModuleId() const
    {
        return moduleId_;
    }

    QString Lesson::getTitle() const
    {
        return title_;
    }

    QString Lesson::getContent() const
    {
        return content_;
    }
}
