#pragma once

#include <QDateTime>
#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief Represents a user's logged answer to a coding task.
     * 
     * A Submission is captured every time a user executes their solution code, 
     * recording the timestamp, the user metadata, problem contexts, and lastly,
     * whether the execution succeeded against all test cases.
     */
    class Submission
    {
    public:
        /**
         * @brief Constructs a new Submission instance.
         * 
         * @param submissionId The unique database identifier of the submission.
         * @param userId The ID answering the task.
         * @param moduleId The ID of the module that housed the coding task.
         * @param codingTaskId The ID of the specific coding task attempted.
         * @param sourceCode The actual code text submitted by the user.
         * @param submittedAt Timestamp representing when the submission was captured.
         * @param isSuccess A cached boolean state of the overall pass/fail status.
         */
        Submission(uint64_t submissionId, uint64_t userId, uint64_t moduleId, uint64_t codingTaskId, QString sourceCode,
                   QDateTime submittedAt, bool isSuccess);

        /**
         * @brief Retrieves the unique identifier of the submission.
         * @return The 64-bit unsigned integer representing the submission ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the submitter's unique identifier.
         * @return The 64-bit unsigned integer representing the user ID.
         */
        uint64_t getUserId() const;

        /**
         * @brief Retrieves the parent module's identifier.
         * @return The 64-bit unsigned integer representing the module ID.
         */
        uint64_t getModuleId() const;

        /**
         * @brief Retrieves the task's identifier.
         * @return The 64-bit unsigned integer representing the specific coding task ID.
         */
        uint64_t getCodingTaskId() const;

        /**
         * @brief Retrieves the submitted code text payload.
         * @return A constant reference to the QString holding the raw source code.
         */
        const QString &getSourceCode() const;

        /**
         * @brief Retrieves the timestamp of the submission attempt.
         * @return A constant reference to the QDateTime object mapped to the attempt time.
         */
        const QDateTime &getSubmittedAt() const;

        /**
         * @brief Indicates if the code submission successfully passed all validations.
         * @return True if the user code succeeded; otherwise false.
         */
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
} // namespace cppforge::entities
