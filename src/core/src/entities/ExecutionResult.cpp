#include "../../include/entities/ExecutionResult.hpp"

namespace cppforge::entities
{
    ExecutionResult::ExecutionResult(uint64_t submissionId, bool isSuccess, const QString &output, const QString &errors,
                                     int32_t timeMs, uint32_t passedTestsCount)
        : submissionId_(submissionId), isSuccess_(isSuccess), output_(output), errors_(errors),
          timeMs_(timeMs), passedTestsCount_(passedTestsCount) {}

    uint64_t ExecutionResult::getSubmissionId() const
    {
        return submissionId_;
    }

    bool ExecutionResult::isSuccess() const
    {
        return isSuccess_;
    }

    QString ExecutionResult::getOutput() const
    {
        return output_;
    }

    QString ExecutionResult::getErrors() const
    {
        return errors_;
    }

    int32_t ExecutionResult::getTimeMs() const
    {
        return timeMs_;
    }

    uint32_t ExecutionResult::getPassedTestsCount() const
    {
        return passedTestsCount_;
    }
}
