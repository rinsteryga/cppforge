#pragma once

#include <cstdint>
#include <QString>
#include <QDateTime>

namespace cppforge
{
    namespace entities
    {
        class Submission
        {
        public:
            Submission(uint64_t id, uint64_t userId, uint64_t moduleId,
                       uint64_t codingTaskId, const QString &sourceCode, 
                       const QDateTime &submittedAt, bool isSuccess);

            uint64_t getId() const;
            uint64_t getUserId() const;
            uint64_t getModuleId() const;
            uint64_t getCodingTaskId() const;
            QString getSourceCode() const;
            QDateTime getSubmittedAt() const;
            bool isSuccess() const;

        private:
            uint64_t id_;
            uint64_t userId_;
            uint64_t moduleId_;
            uint64_t codingTaskId_;
            QString sourceCode_;
            QDateTime submittedAt_;
            bool isSuccess_;
        };
    } // namespace entities
} // namespace cppforge
