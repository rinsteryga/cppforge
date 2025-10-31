#pragma once

#include <cstdint>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class ExecutionResult
        {
        public:
            ExecutionResult(bool isSuccess, const QString &output, const QString &errors,
                            int32_t timeMs, uint32_t passedTestsCount);

            bool isSuccess() const;
            QString getOutput() const;
            QString getErrors() const;
            int32_t getTimeMs() const;
            uint32_t getPassedTestsCount() const;

        private:
            bool isSuccess_;
            QString output_;
            QString errors_;
            int32_t timeMs_;
            uint32_t passedTestsCount_;
        };
    } // namespace entities
} // namespace cppforge
