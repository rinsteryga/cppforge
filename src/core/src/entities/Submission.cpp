#include "../../include/entities/Submission.hpp"

#include <utility>

namespace cppforge::entities
{
    Submission::Submission(uint64_t submissionId, uint64_t userId, uint64_t moduleId, uint64_t codingTaskId,
                           QString sourceCode, QDateTime submittedAt, bool isSuccess)
        : id_(submissionId), userId_(userId), moduleId_(moduleId), codingTaskId_(codingTaskId),
          sourceCode_(std::move(sourceCode)), submittedAt_(std::move(submittedAt)), isSuccess_(isSuccess)
    {
    }

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

    const QString &Submission::getSourceCode() const
    {
        return sourceCode_;
    }

    const QDateTime &Submission::getSubmittedAt() const
    {
        return submittedAt_;
    }

    bool Submission::isSuccess() const
    {
        return isSuccess_;
    }
} // namespace cppforge::entities
