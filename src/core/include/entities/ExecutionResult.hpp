#pragma once

#include <QString>

#include <cstdint>

namespace cppforge
{
    namespace entities
    {
        class ExecutionResult
        {
        public:
            ExecutionResult() = default;
            ExecutionResult(uint64_t submissionId, bool isSuccess, const QString &output, const QString &errors,
                            int32_t timeMs, uint32_t passedTestsCount);

            uint64_t getSubmissionId() const;
            bool isSuccess() const;
            const QString &getOutput() const;
            const QString &getErrors() const;
            int32_t getTimeMs() const;
            uint32_t getPassedTestsCount() const;

        private:
            uint64_t submissionId_;
            bool isSuccess_;
            QString output_;
            QString errors_;
            int32_t timeMs_;
            uint32_t passedTestsCount_;
        };
    } // namespace entities
} // namespace cppforge
