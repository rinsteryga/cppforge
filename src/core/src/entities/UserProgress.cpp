#include "../../include/entities/UserProgress.hpp"

namespace cppforge::entities
{
    UserProgress::UserProgress(uint64_t userId, uint64_t moduleId, uint64_t lessonId, bool isCompleted)
        : userId_(userId), moduleId_(moduleId), lessonId_(lessonId), isCompleted_(isCompleted) {}

    uint64_t UserProgress::getUserId() const
    {
        return userId_;
    }

    uint64_t UserProgress::getModuleId() const
    {
        return moduleId_;
    }

    uint64_t UserProgress::getLessonId() const
    {
        return lessonId_;
    }

    bool UserProgress::isCompleted() const
    {
        return isCompleted_;
    }

    void UserProgress::setCompleted(bool completed)
    {
        isCompleted_ = completed;
    }
}
