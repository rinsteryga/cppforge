#include "../../include/entities/Submission.hpp"

namespace cppforge::entities
{
    Submission::Submission(uint64_t id, uint64_t userId, uint64_t moduleId, uint64_t codingTaskId,
                           const QString &sourceCode, const QDateTime &submittedAt, bool isSuccess)
        : id_(id), userId_(userId), moduleId_(moduleId), codingTaskId_(codingTaskId),
          sourceCode_(sourceCode), submittedAt_(submittedAt), isSuccess_(isSuccess) {}

    uint64_t Submission::getId() const
    {
        return id_;
    }

    uint64_t Submission::getUserId() const
    {
        return userId_;
    }

    uint64_t Submission::getModuleId() const
    {
        return moduleId_;
    }

    uint64_t Submission::getCodingTaskId() const
    {
        return codingTaskId_;
    }

    QString Submission::getSourceCode() const
    {
        return sourceCode_;
    }

    QDateTime Submission::getSubmittedAt() const
    {
        return submittedAt_;
    }

    bool Submission::isSuccess() const
    {
        return isSuccess_;
    }
}
